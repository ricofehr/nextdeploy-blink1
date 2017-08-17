# nextdeploy-blink1

Manages a [blink1 device](https://blink1.thingm.com/) for some [NextDeploy](https://github.com/ricofehr/nextdeploy) supervise alerts.

## Prerequisites

Needs Cmake (>3.1), Jansson and curl libraries, and the hidapi library from [blink1 repo](https://github.com/todbot/blink1).

Install Jansson Library
```
$ git clone https://github.com/akheron/jansson
$ cd jansson
$ autoreconf -i
$ ./configure
$ make
$ sudo make install
```


Install hidapi blink1 version
```
$ git clone https://github.com/todbot/blink1
$ cd blink1/commandline/hidapi
$ ./bootstrap
$ ./configure
$ make
$ sudo make install
```

libcrul may be isntalled via your package manager (apt, brew, yum, ...)


## Compile

```
$ cd build && cmake ..
$ make
```

## Folders

```
+--bin/		Binary folder where nextfloor executable is written
+--build/   Build folder for compile stuffs
+--cmake/   Cmake modules folder
+--config/  Config folder
+--doxy/    Destination folder for documentation generated
+--src/ 	Sources
+--tmp/     Temporary folder
```

The folder src/ndeployblink1/blink1/lib is the Blink1 C Api Library duplicated from the [blink1 repo](https://github.com/todbot/blink1)

## Settings

A default setting file is present at config/nextdeploy.conf.default
For local config change, we can duplicate this file
```
$ cp config/nextdeploy.conf.default config/nextdeploy.conf
$ vi config/nextdeploy.conf.default
```

This file must include your NextDeploy username, password and the hostname of the api.

## Run

The program runs in an infinite loop (ctr-c for stop the loop) and check NextDeploy vms status every 20 seconds.
```
$ ./bin/./ndeployblink1
```

At each loop, the color of the blink1 device is updated as follows
- Green: all is ok
- Blue: a new vm is currently on setup state
- Orange: one or more vm services are down (details on cmd output)
- Red: one or more Vm are on error state
- Purple: a network, credentials, or json issue with the remote api

## Documentation

For generate doxygen documentation, into the project folder
```
doxygen doxygen.config
```
