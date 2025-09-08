#include <errno.h>
#include <fcntl.h>
#include <net/if_utun.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/kern_control.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sys_domain.h>
#include <sys/types.h>
#include <sys/un.h>
#include <syslog.h>
#include <termios.h>
#include <unistd.h>

#define DEVICE_TYPE_HARDWARE 'h'
#define DEVICE_TYPE_SOCKET_CLIENT 'c'
#define DEVICE_TYPE_SOCKET_SERVER 's'
#define DEFAULT_BAUD 9600

#define MAX_UTUN_NUMBER 255

#define MTU 1500
#define NULL_LOOPBACK_HEADER_SIZE 4
#define MAX_PACKET_SIZE (MTU + NULL_LOOPBACK_HEADER_SIZE)
#define MAX_PACKET_SIZE_SLIP                                                   \
    (MTU * 2 + 1) // worst case all escaped plus the end character

#define END 0xc0
#define ESC 0xdb
#define ESC_ESC 0xdd
#define ESC_END 0xdc

#define DECODE_END_OF_PACKET -2

// #define DEBUG

int open_serial_port(const char *device, uint32_t baud_rate) {
    // From: https://www.pololu.com/docs/0J73/15.5
    // Opens the specified serial port, sets it up for binary communication,
    // configures its read timeouts, and sets its baud rate.
    // Returns a non-negative file descriptor on success, or -1 on failure.
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror(device);
        return -1;
    }

    // Flush away any bytes previously read or written.
    int result = tcflush(fd, TCIOFLUSH);
    if (result) {
        perror("tcflush failed"); // just a warning, not a fatal error
    }

    // Get the current configuration of the serial port.
    struct termios options;
    result = tcgetattr(fd, &options);
    if (result) {
        perror("tcgetattr failed");
        close(fd);
        return -1;
    }

    // Turn off any options that might interfere with our ability to send and
    // receive raw binary bytes.
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    // Set up timeouts: Calls to read() will return as soon as there is
    // at least one byte available or when 100 ms has passed.
    // options.c_cc[VTIME] = 0;//1;
    // options.c_cc[VMIN] = 0;

    // This code only supports certain standard baud rates. Supporting
    // non-standard baud rates should be possible but takes more work.
    switch (baud_rate) {
    case 4800:
        cfsetospeed(&options, B4800);
        break;
    case 9600:
        cfsetospeed(&options, B9600);
        break;
    case 19200:
        cfsetospeed(&options, B19200);
        break;
    case 38400:
        cfsetospeed(&options, B38400);
        break;
    case 115200:
        cfsetospeed(&options, B115200);
        break;
    default:
        fprintf(stderr, "warning: baud rate %u is not supported, using 9600.\n",
                baud_rate);
        cfsetospeed(&options, B9600);
        break;
    }
    cfsetispeed(&options, cfgetospeed(&options));

    result = tcsetattr(fd, TCSANOW, &options);
    if (result) {
        perror("tcsetattr failed");
        close(fd);
        return -1;
    }

    return fd;
}

int open_unix_domain_socket_as_client(const char *socket_path,
                                      int error_is_fatal) {
    // From: https://troydhanson.github.io/network/Unix_domain_sockets.html

    int fd;
    struct sockaddr_un addr;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (*socket_path == '\0') {
        *addr.sun_path = '\0';
        strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
    } else {
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    }

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        if (error_is_fatal) {
            perror("connect error");
        }
        close(fd);
        return -1;
    }

    return fd;
}

int open_unix_domain_socket_as_server(const char *socket_path) {
    // From: https://troydhanson.github.io/network/Unix_domain_sockets.html

    int fd, cl;
    struct sockaddr_un addr;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    // Make the socket world writable on the assumption you'll want to connect
    // to it from a process running as non-root. chmod on a socket doesn't work
    // on MacOS so we have to temporarily change the umask instead.
    mode_t orig_umask = umask(0000);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (*socket_path == '\0') {
        *addr.sun_path = '\0';
        strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
    } else {
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
        unlink(socket_path);
    }

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind error");
        exit(-1);
    }

    if (listen(fd, 5) == -1) {
        perror("listen error");
        exit(-1);
    }

    // Put umask back just in case
    umask(orig_umask);

    printf("Socket opened, waiting for client connect...\n");

    if ((cl = accept(fd, NULL, NULL)) == -1) {
        perror("accept error");
        return -1;
    }

    return cl;
}

int tun(int number) {
    // Original header:
    // From http://newosxbook.com/src.jl?tree=listings&file=17-15-utun.c
    //   via
    //   https://github.com/OpenVPN/openvpn/blob/cbc3c5a9831b44ec7f59e8cb21e19ea364e6c0ee/src/openvpn/tun.c
    // Simple User-Tunneling Proof of Concept - extends listing 17-15 in book
    //
    // Compiles for both iOS and OS X..
    //
    // Coded by Jonathan Levin. Go ahead; Copy, improve - all rights allowed.
    //
    //  (though credit where credit is due would be nice ;-)

    struct sockaddr_ctl sc;
    struct ctl_info ctlInfo;
    int fd;

    memset(&ctlInfo, 0, sizeof(ctlInfo));
    if (strlcpy(ctlInfo.ctl_name, UTUN_CONTROL_NAME,
                sizeof(ctlInfo.ctl_name)) >= sizeof(ctlInfo.ctl_name)) {
        fprintf(stderr, "UTUN_CONTROL_NAME too long");
        return -1;
    }
    fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);

    if (fd == -1) {
        perror("socket(SYSPROTO_CONTROL)");
        return -1;
    }
    if (ioctl(fd, CTLIOCGINFO, &ctlInfo) == -1) {
        perror("ioctl(CTLIOCGINFO)");
        close(fd);
        return -1;
    }

    sc.sc_id = ctlInfo.ctl_id;
    sc.sc_len = sizeof(sc);
    sc.sc_family = AF_SYSTEM;
    sc.ss_sysaddr = AF_SYS_CONTROL;
    sc.sc_unit = number + 1;

    // If the connect is successful, a tun%d device will be created, where "%d"
    // is our unit number -1

    if (connect(fd, (struct sockaddr *)&sc, sizeof(sc)) == -1) {
        // Note if the utun is already in use this check will be hit,
        // we don't log this as it's not an isue, we can try another
        // one
        // perror ("connect(AF_SYS_CONTROL)");
        close(fd);
        return -1;
    }

    return fd;
}

int encode_slip(unsigned char *in, unsigned char *out, int length) {
    int count = 0;
    int i;

    for (i = 0; i < length; i++) {
        unsigned char c = *in;
        if (c == END) {
            *out = ESC;
            out++;
            *out = ESC_END;
            count++;
        } else if (c == ESC) {
            *out = ESC;
            out++;
            *out = ESC_ESC;
            count++;
        } else {
            *out = c;
        }

        in++;
        out++;
        count++;
    }

    *out = END;
    count++;

    return count;
}

int decode_slip(int fd) {
    unsigned char c;
    if (!read(fd, &c, 1)) {
        printf("Read error\n");
        return -1;
    }

    if (c == ESC) {
        // escape character - look at the next one too
        if (!read(fd, &c, 1)) {
            printf("Read error\n");
            return -1;
        }
        if (c == ESC_END) {
            return END;
        } else if (c == ESC_ESC) {
            return ESC;
        } else {
            printf("Decoding error\n");
            return -1;
        }
    } else if (c == END) {
        return DECODE_END_OF_PACKET;
    } else {
        return c;
    }
}

int next_slip_packet(int fd, unsigned char buf[MTU]) {
    int i = 0;
    while (1) {
        int result = decode_slip(fd);
        if (result == -1) {
            return -1;
        } else if (result == DECODE_END_OF_PACKET) {
            // full packet
            return i;
        } else {
            buf[i] = (unsigned char)result;
            i++;
        }
    }
}

typedef struct thread_args {
    int utunfd;
    int serialfd;
} thread_args;

void *tx_thread(void *vargp) {
    thread_args *args = (thread_args *)vargp;

    // Read from tunnel and forward it to serial
    while (1) {
        unsigned char c[MAX_PACKET_SIZE];
        unsigned char encoded[MAX_PACKET_SIZE_SLIP];
        int len;
        int i;
        int encoded_length;

        len = read(args->utunfd, c, MAX_PACKET_SIZE);

        if (len == -1) {
            printf("error %i\n", errno);
            return vargp;
        }

        // Skip first 4 bytes - this is the null/loopback header
        len -= 4;
        unsigned char packet[MTU];
        memcpy(packet, &c[4], len);

        encoded_length = encode_slip(packet, encoded, len);

#ifdef DEBUG
        printf("TX:\n");
        for (i = 0; i < encoded_length; i++) {
            printf("%02x ", encoded[i]);
            if ((i - 4) % 16 == 15)
                printf("\n");
        }
        printf("\n");
#endif

        write(args->serialfd, encoded, encoded_length);
    }
    return vargp;
}

void *rx_thread(void *vargp) {
    thread_args *args = (thread_args *)vargp;

    unsigned char c[MTU];
    unsigned char packet[MAX_PACKET_SIZE];

    packet[0] = 0;
    packet[1] = 0;
    packet[2] = 0;
    packet[3] = AF_INET;

    // Read from serial and forward it to tunnel
    while (1) {
        int length;
        length = next_slip_packet(args->serialfd, c);
        if (length < 0) {
            return vargp;
        } else if (length < 1) {
            continue;
        }
        // Copy into packet. The first 4 bytes of packet are static
        // and remain the same, so we copy after them
        memcpy(&packet[4], c, length);
        length += 4;

#ifdef DEBUG
        printf("RX:\n");
        for (int i = 0; i < length; i++) {
            printf("%02x ", packet[i]);
            if ((i - 4) % 16 == 15)
                printf("\n");
        }
        printf("\n");
#endif

        write(args->utunfd, packet, length);
    }
    return vargp;
}

int create_utun(int *utun_num) {
    int num = 0;

    int fd = -1;

    while (num < MAX_UTUN_NUMBER) {
        int result = tun(num);
        if (result != -1) {
            fd = result;
            break;
        }
        num++;
    }

    if (fd == -1) {
        fprintf(stderr, "Unable to create UTUN. Are you root?\n");
        exit(1);
    }

    printf("Created utun%i\n", num);

    *utun_num = num;

    return fd;
}

int connect_device(char device_type, char *device_path, int baud,
                   int error_is_fatal) {
    int fd;

    while (1) {
        switch (device_type) {
        case DEVICE_TYPE_HARDWARE:
            fd = open_serial_port(device_path, baud);
            break;
        case DEVICE_TYPE_SOCKET_CLIENT:
            fd = open_unix_domain_socket_as_client(device_path, error_is_fatal);
            break;
        case DEVICE_TYPE_SOCKET_SERVER:
            fd = open_unix_domain_socket_as_server(device_path);
            break;
        }

        if (fd == -1) {
            if (error_is_fatal) {
                fprintf(stderr, "Unable to open device\n");
                exit(1);
            } else {
                continue;
            }
        } else {
            return fd;
        }
    }
}

void run_ifconfig(int utun_num, char *local_ip, char *remote_ip) {
    char ifconfig[50];

    sprintf(ifconfig, "ifconfig utun%i %s %s", utun_num, local_ip, remote_ip);

    printf("Running: %s\n", ifconfig);
    if (system(ifconfig) != 0) {
        fprintf(stderr, "ifconfig failed\n");
        exit(1);
    }
}

int main(int argc, char **argv) {
    thread_args thread_args;

    char *device_path;
    char *local_ip;
    char *remote_ip;
    int baud = DEFAULT_BAUD;
    char device_type = DEVICE_TYPE_HARDWARE;

    int opt;

    while ((opt = getopt(argc, argv, "b:l:r:t:")) != -1) {
        switch (opt) {
        case 'b':
            baud = atoi(optarg);
            break;
        case 'l':
            local_ip = optarg;
            break;
        case 'r':
            remote_ip = optarg;
            break;
        case 't':
            device_type = optarg[0];
            break;
        }
    }

    if (optind < argc) {
        device_path = argv[optind];
    }

#ifdef DEBUG
    printf("Device: %s Type: %c Local: %s Remote: %s Baud: %i\n", device_path,
           device_type, local_ip, remote_ip, baud);
#endif

    if (!(device_type == DEVICE_TYPE_HARDWARE ||
          device_type == DEVICE_TYPE_SOCKET_SERVER ||
          device_type == DEVICE_TYPE_SOCKET_CLIENT) ||
        !local_ip || !remote_ip || !device_path) {
        fprintf(
            stderr,
            "Usage: %s -l local_ip -r remote_ip [-b baud] [-t type] [device]\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }

    int utun_num;

    thread_args.utunfd = create_utun(&utun_num);

    run_ifconfig(utun_num, local_ip, remote_ip);

    // The first time we try to open the device any error should be fatal
    // as this is likely a config problem.
    // After that we will try to reconnect in the event of an error, for
    // example due to serial line being disconnected, socket server restart
    // etc.
    thread_args.serialfd = connect_device(device_type, device_path, baud, 1);

    pthread_t tx_thread_id, rx_thread_id;

    pthread_create(&tx_thread_id, NULL, tx_thread, (void *)&thread_args);

    while (1) {
        pthread_create(&rx_thread_id, NULL, rx_thread, (void *)&thread_args);

        printf("SLIP connection up\n");

        pthread_join(rx_thread_id, NULL);

        printf("Device lost, attempting reconnect...\n");

        thread_args.serialfd = connect_device(device_type, device_path, baud, 0);
    }

    return 0;
}