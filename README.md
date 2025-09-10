# Running a web server over SWD
After playing around with semihosting on a little py32 microcontroller I found in a disposable vape, I realised that pyocd can forward the semihosting stdout/stdin to a TCP port.

Considering that all of the internet used to be run over serial modems, once you can send and receive data to a device, you can talk TCP/IP.

This is where using UART would have actually made things easier, but I wanted to see if I could do it over SWD. Most modem drivers expect to see a serial device(`/dev/ttyACM0` or similar), but we can make any unix socket look like a serial device using `socat`:
```sh
socat PTY,link=./tty,raw,echo=0,wait-slave TCP:localhost:$(PORT),nodelay
```

Now we have a `./tty` device that we can use as a serial device. This is also a good trick if you prefer to use something like `picocom` or `minicom` to interact with serial devices.

Ok, so we have a serial device, how do we run TCP/IP over it? We need a network layer because we are not running over Ethernet or WiFi.
The simples protocol I have come across is SLIP (Serial Line Internet Protocol). It is less performant than PPP, but it's very simple to set up, and as a bonus, the IP stack I am using (uIP) has SLIP support built in.

I wrote this project on a mac, so I used a different method to create a SLIP interface than you would on Linux, but they essentially do the same thing.

> [!WARNING]
> I didn't test this on Linux.

Here is the Linux command:
```sh
sudo slattach -p slip -s 115200 ./tty &
sudo ip link set sl0 up
sudo ip addr add 192.168.190.1 peer 192.168.190.2 dev sl0
```

For the TCP/IP stack, I used uIP v9.0 as it is pretty popular and easy to port as well as being very small and having good examples. I used the picosoc-uip project as a reference for how to set up a minimal web server. My only change was to optimise the SLIP serialisation just a little bit to reduce the semihosting overhead.

And that's it! You can now ping `192.168.190.2` or curl `http://192.168.190.2:80` to see the web server running on the microcontroller.

The resource usage is pretty low, the RAM was pushed as far as I could, it can be reduced way more:
```
Memory region         Used Size  Region Size  %age Used
           FLASH:        8528 B        24 KB     34.70%
             RAM:        2672 B         3 KB     86.98%
```


# Aknowledgements
 - [pyOCD](https://github.com/pyocd/pyOCD)
 - [uIP](https://github.com/adamdunkels/uip/tree/uip-0-9)
 - [picosoc-uip](https://github.com/grahamedgecombe/picosoc-uip)
 - [slip-macos](https://github.com/jackqu7/slip-macos)
 - [wagiminator's MCU Templates](https://github.com/wagiminator/MCU-Templates)
