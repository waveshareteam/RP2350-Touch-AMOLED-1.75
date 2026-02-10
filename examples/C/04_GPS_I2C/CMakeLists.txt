set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(main C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(main
        main.c
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# pull in common dependencies and additional i2c hardware support
target_link_libraries(main pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(main)

# add url via pico_set_program_url
