# STM32VLDiscovery for Linux

There is a template STM32VLDiscovery project for Linux.

Unlike other similar projects, there are:

* You don't need Keil
* You don't need IAR
* You don't need Eclipse or Eclipse-based IDE
* You don't need any another IDE!

All what you need - is a blank console, emacs or vim for source code editing and other dependencies:

* Cross-compiler (arm-none-eabi-gcc, the same gdb and etc). You should use gcc-arm-none-eabi and gdb-arm-none-eabi packages if you are user of Debian-based distro.
* stlink utility (https://github.com/texane/stlink) - to load binary to the STM32VLDiscovery board.
* make
* sudo
* Development files for SG utils (install ``libsgutils2-dev`` for Debian-based distributives or ``sg3-utils`` for Gentoo-based).

## Compiling the project

To compile project - just execute ``make`` in console.

## Loading project to STM32VLDiscovery

Just type ``make load`` in console and follow instructions on the screen. If something is missing - my script will help you with dependencies.

