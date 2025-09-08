# Serial Line IP (SLIP) for MacOS

This program allows you to use a SLIP connection on MacOS, similar to the `slattach` command on Linux. It will create a tunneled network device and forward all packets to/from it to the serial port using SLIP encoding. If the connection is lost it will attempt to reconnect automatically.

Unlike earlier implementations, this program uses the native utun device thus avoiding the need for a kernel extension. This means it works on Big Sur and Apple Silicon.

I wrote this program to communicate with a hobby embedded system. Though it seems to work fine, I cannot guarantee it is free of bugs or security issues and you should therefore not use it for any important.

## Building
Clone repo and
```
make
```
This will build the program as `./slip`

## Usage
```
sudo ./slip -l 192.168.190.1 -r 192.168.190.2 /dev/cu.usbserial-XXX
```

Creates connection using the specified USB serial port at 9600 baud. Mac to use 192.168.190.1 and remote device to use 192.168.190.2.

Note the program must be run as root in order to create a utun device.

### Options

* `-b 9600` baud rate - 4800/9600/19200/38400/115200
* `-l 192.168.190.1` IP address your Mac should use
* `-r 192.168.190.2` IP address of remote device
* `/dev/cu.usbserial-XXX` Serial device to use, or (relative/absolute) path to socket if using Unix Domain Sockets

Device Types:
* `-t h` Hardware serial port (via USB) - I use this to communicate with an embedded system
* `-t s` Unix Domain Socket (server) - I use this with the emulator for the embedded system
* `-t c` Unix Domain Socket (client) - You can run two instances for testing - one in server mode and one in client. Also works with socket serial ports exposed from Parallels VMs, though I have no idea why you would ever want to do that.

## Internet Connection Sharing

If you would like to share your internet connection with the SLIP device, this can be done using the built in internet connection sharing in MacOS.

Edit `/etc/pf.conf` to add the two lines in bold.

Note that order is important in this file, so if your version doesn't look exactly like this add the lines in approximately the same place.

Replace `utun0` with the device this program created and replace `en0` with your internet device if needed.

<pre>
scrub-anchor "com.apple/*"
nat-anchor "com.apple/*"
<b>nat on en0 from utun0:network to any -> (en0)</b>
rdr-anchor "com.apple/*"
<b>pass on utun0 all flags any keep state rtable 6</b>
dummynet-anchor "com.apple/*"
anchor "com.apple/*"
load anchor "com.apple" from "/etc/pf.anchors/com.apple"
</pre>

Now run:
```
sudo sysctl -w net.inet.ip.forwarding=1
sudo pfctl -d
sudo pfctl -e -f /etc/pf.conf
```

## References

[Jonathan Levin utun example](http://newosxbook.com/src.jl?tree=listings&file=17-15-utun.c)

[Python implementation](https://github.com/antoinealb/serial-line-ip-osx) - requires kernel extension

[Pololu serial port example](https://www.pololu.com/docs/0J73/15.5)

[Troy D. Hanson unix domain sockets example](https://troydhanson.github.io/network/Unix_domain_sockets.html)

[slattach Linux command](https://linux.die.net/man/8/slattach)
