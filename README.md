# MiniTracker

a handheld hardware tracker using the Daisy Seed inspired by Renoise.

## Installation and loading

Follow [this guide](https://daisy.audio/tutorials/cpp-dev-env/#3-vs-code) to intall the Daisy toolchain on you system and in you IDE of choice.
Flash the [bootloader to the Seed](https://daisy.audio/tutorials/_a7_Getting-Started-Daisy-Bootloader/), build the project, then flash to QSPI with 'make program-dfu'.
You may need to change some code so that your pin setup is correct.

## TODO

- Add sd card sample loading capabilities
- Add project saving and loading capabilities
- Add song setting modification
- Optimize DMA RAM usage
