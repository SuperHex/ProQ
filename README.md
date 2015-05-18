Project Utopia
---

Note: This is the experimental branch of Utopia,you should NOT use it unless for develop.Use stable branch instead.
Utopia is a experimantal quadcopter flight controller project. We start this project in order to learn how to build a quadcopter from scratch, and make it as easy as possiable.
There are serval modules we are going to build,include:
- [ ] main.c - main controller
   - [ ] utility - some useful tools to get access to mcu and sensors.
   - [ ] mathematics - algorithms that keep things running.
   - [ ] driver - After getting some results, apply it to motors.
   - [ ] sensors - tools for sensors.

Usage
---

First, make sure you have installed the required tools.
```
$ sudo apt-get install gcc-avr avr-libc avrdude
```

build from source code
```
$ git clone https://github.com/SuperHex/ProQ
$ cd ./ProQ
$ make
```

and you can upload your `.hex` file by avrdude
```
$ avrdude -p m168 -P /dev/ttyUSB0 -c stk500v1 -b 19200 -F -u -U flash:w:foo.hex  # change "foo.hex" to what you get
```

