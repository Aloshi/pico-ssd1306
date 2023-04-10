# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-ssd1306 C CXX ASM)

# Initialise the Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pico-ssd1306 main.cpp ssd1306.h ssd1306.cpp image_common.h data/blink.h data/blink.cpp)

pico_set_program_name(pico-ssd1306 "pico-ssd1306")
pico_set_program_version(pico-ssd1306 "0.1")

pico_enable_stdio_uart(pico-ssd1306 0)
pico_enable_stdio_usb(pico-ssd1306 1)

# Add the standard library to the build
target_link_libraries(pico-ssd1306 pico_stdlib)

# Add any user requested libraries
target_link_libraries(pico-ssd1306
        hardware_spi
        hardware_dma
        hardware_pio
        hardware_timer
        hardware_clocks
        )

pico_add_extra_outputs(pico-ssd1306)
