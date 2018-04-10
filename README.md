# CANtact firmware

This repository contains an alternate firmware for the CANtact USB-CAN adapter.<br/>
It has for example the following advantages compared to the original firmware:

* Interrupts are used for CAN reception
* Enhanced frame buffering
* Can also be compiled for Nucleo-F042

The original firmware repository can be found [here](https://github.com/linklayer/cantact-fw).<br/>
The official CANtact documentation can be found on the [Linklayer Wiki](https://wiki.linklayer.com/index.php/CANtact).

## Building

Firmware builds with GCC. Specifically, you will need gcc-arm-none-eabi, which
is packaged for Windows, OS X, and Linux on
[Launchpad](https://launchpad.net/gcc-arm-embedded/+download). Download for your
system and add the `bin` folder to your PATH.

With that done, you should be able to compile using:
```sh
make
```

## Flashing & Debugging

Firmware images (see releases tab) can be flashed onto the CANtact adapter
either using an SWD debugging adapter
or by putting a jumper on the BOOT pins and using dfu-util or ST's DfuSe utility.
Debugging can be only be done with an SWD debugging adapter,
such as the STLinkv2 USB-SWD adapters
present on any STM32 Discovery or Nucleo board.

### dfu-util

After installting dfu-util via your preferred package manager or from [here](https://sourceforge.net/projects/dfu-util/files/),
you can use the following Makefile target for firmware upload:
```sh
make flash
```

### ST's DfuSe utility

After downloading and installing the software,
use the generator to create a .dfu from the .bin
with target address 0x08000000.
Then use the DfuSe Demo to flash it onto the CANtact.
A short tutorial can be found here:

https://blog.matthiasbock.net/2018/04/05/flashing-a-cantact-firmware-using-sts-dfuse-client/

### OpenOCD
To use an STLinkv2 for debugging, install and run [OpenOCD](http://openocd.sourceforge.net/):
```sh
openocd -f interface/stlink.cfg -f target/stm32f0x.cfg
```

With OpenOCD running, arm-none-eabi-gdb can be used to load code and debug.

## Contributors

- [Matthias Bock](https://github.com/matthiasbock) - Major code rework
- [Ethan Zonca](https://github.com/normaldotcom) - Makefile fixes and code size optimization
- [onejope](https://github.com/onejope) - Fixes to extended ID handling
- Phil Wise - Added dfu-util compatibility to Makefile
- [Eric Evenchik](https://github.com/ericevenchick) - Original author

## License

See LICENSE.md
