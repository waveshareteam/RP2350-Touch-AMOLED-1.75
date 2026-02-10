set(PICO_BOARD pico2 CACHE STRING "Board type")
cmake_minimum_required(VERSION 3.27)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(main C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(main)

pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/lib/QSPI_PIO/qspi.pio
                        OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/QSPI_PIO)

target_sources(main PRIVATE
    main.c
)

# Add compilation subdirectory
add_subdirectory(./lib/Config)
add_subdirectory(./lib/QSPI_PIO)
add_subdirectory(./lib/AMOLED)
add_subdirectory(./lib/GUI)
add_subdirectory(./lib/Fonts)
add_subdirectory(./lib/QMI8658)
add_subdirectory(./lib/Touch)
add_subdirectory(./lib/AXP2101)

# Add header file directory
include_directories(./lib/Config)
include_directories(./lib/QSPI_PIO)
include_directories(./lib/AMOLED)
include_directories(./lib/GUI)
include_directories(./lib/Fonts)
include_directories(./lib/QMI8658)
include_directories(./lib/Touch)
include_directories(./lib/AXP2101)

target_link_libraries(main PRIVATE 
    Config
    QSPI_PIO
    AMOLED
    GUI
    Fonts
    QMI8658
    Touch
    AXP2101
    pico_stdlib 
    hardware_pio 
    hardware_pwm 
    hardware_dma
    hardware_spi
    hardware_i2c
    hardware_gpio
    hardware_adc
)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

pico_add_extra_outputs(main)

# add url via pico_set_program_url

