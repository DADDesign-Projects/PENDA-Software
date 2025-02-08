# PENDA-Sofware
The **PENDA Software Framework** is a development toolkit for the **PENDA hardware platform**, designed to simplify the creation of effect pedals using the **Daisy Seed** board. Developed with **STM32CubeIDE**, this framework handles essential components like audio processing, memory management, and display support, providing developers with a solid base to build on.

The primary goal is to offer a comprehensive starting point for developers, allowing them to focus on the creative aspects of pedal development without worrying about low-level hardware configuration.


## Author
This project is developed by DAD Design.

## License
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)


## Features

This project provides a basic framework for programming the **Daisy Seed** board, with a focus on the PENDA hardware platform. It includes a complete configuration of essential hardware components, such as:

- **Audio Codec**: Handles initialization and audio interface management for seamless audio processing.  
- **SDRAM Memory**: Provides configuration and access to external SDRAM for efficient data storage.  
- **QSPI Flash Memory**: Manages QSPI memory, including erase, write, and read operations, for fast non-volatile storage.  
- **TFT Display**: Supports drivers for **ST7789** or **ST7735** displays, enabling graphical user interface (GUI) integration.  
- **Graphics Library**: Integration of **DaisySeedGFX2** for easy UI creation and visualization, allowing developers to design custom displays.
- ...

## References

- **PENDA-Hardware repository**: [https://github.com/DADDesign-Projects/PENDA-Hardware](https://github.com/DADDesign-Projects/PENDA-Hardware)
- **DaisySeedGFX2 repository**: [https://github.com/DADDesign-Projects/DaisySeedGFX2](https://github.com/DADDesign-Projects/DaisySeedGFX2)
 
