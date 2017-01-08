# neopixel 7-segment clock

building a digital clock from scratch
using 30 neopixels (4 × 7 segments + 2 dots)

# thanks

-  ["DSEG" font](http://www.keshikan.net/fonts-e.html)
(licensed under [SIL OPEN FONT LICENSE Version 1.1](http://scripts.sil.org/OFL), [source](https://github.com/keshikan/DSEG))

- [Arduino](https://www.arduino.cc)

- [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)

- [Arduino Time Library by Michael Margolis](https://www.pjrc.com/teensy/td_libs_Time.html)

- [vemeT5ak](https://github.com/vemeT5ak) for [code inspiration](https://github.com/vemeT5ak/7-segment-neopixel-clock)

# sync from OpenWRT router

clock/arduino is connected to router via USB,
also provides power.

## prepare router

```sh
opkg update && opkg install kmod-usb-serial-ftdi
dmesg
```

should output
```
…
[341699.646449] usbcore: registered new interface driver ftdi_sio
[341699.646494] usbserial: USB Serial support registered for FTDI USB Serial Device
[341699.646655] ftdi_sio 2-1:1.0: FTDI USB Serial Device converter detected
[341699.646723] usb 2-1: Detected FT232RL
[341699.646933] usb 2-1: FTDI USB Serial Device converter now attached to ttyUSB0
```

## sync daemons for openWRT

prepare:

```sh
opkg install bc socat
rsync -avPh router-sync/ root@10.178.84.1:/ && \
ssh root@10.178.84.1 '/etc/init.d/arduino-socket enable && /etc/init.d/arduino-socket start && /etc/init.d/serial-clock-sync enable && /etc/init.d/serial-clock-sync start'
```


## proxy serial port for development

(WORK IN PROGRESS)

```sh
# server (router):
opkg install socat
socat tcp-l:1234,reuseaddr,fork file:/dev/ttyUSB0,nonblock,raw,echo=0,waitlock=/var/run/tty,b9600

# client (dev machine):
mkdir $HOME/dev
socat pty,link=$HOME/dev/vmodem0,waitslave tcp:router.local:1234
```
