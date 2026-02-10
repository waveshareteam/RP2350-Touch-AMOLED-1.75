cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(RP2350-Touch-AMOLED-1.75-LVGL C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

# Add compilation subdirectory
add_subdirectory(./examples)
add_subdirectory(./lib/Config)
add_subdirectory(./lib/QSPI_PIO)
add_subdirectory(./lib/AMOLED)
add_subdirectory(./lib/Touch)
add_subdirectory(./lib/QMI8658)
add_subdirectory(./lib/PCF85063A)
add_subdirectory(./lib/lvgl)
add_subdirectory(./lib/PSRAM)

# Add header file directory
include_directories(./examples)
include_directories(./lib/Config)
include_directories(./lib/QSPI_PIO)
include_directories(./lib/AMOLED)
include_directories(./lib/Touch)
include_directories(./lib/QMI8658)
include_directories(./lib/PCF85063A)
include_directories(./lib/lvgl)
include_directories(./lib/PSRAM)
include_directories(./examples/inc)

# PICO_CONFIG: PICO_XOSC_STARTUP_DELAY_MULTIPLIER, Multiplier to lengthen xosc startup delay to accommodate slow-starting oscillators, type=int, min=1, default=1, group=hardware_xosc

add_executable(RP2350-Touch-AMOLED-1.75-LVGL main.c)

pico_generate_pio_header(RP2350-Touch-AMOLED-1.75-LVGL ${CMAKE_CURRENT_LIST_DIR}/lib/QSPI_PIO/qspi.pio
                        OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/QSPI_PIO)


pico_set_program_name(RP2350-Touch-AMOLED-1.75-LVGL "RP2350-Touch-AMOLED-1.75-LVGL")
pico_set_program_version(RP2350-Touch-AMOLED-1.75-LVGL "0.1")

pico_enable_stdio_uart(RP2350-Touch-AMOLED-1.75-LVGL 0)
pico_enable_stdio_usb(RP2350-Touch-AMOLED-1.75-LVGL 1)

# Add the standard library to the build

set(LV_CONF_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/LVGL/lv_conf.h
    CACHE STRING "" FORCE)
add_compile_options(-O2)


target_link_libraries(RP2350-Touch-AMOLED-1.75-LVGL
                      Config
                      QSPI_PIO
                      AMOLED
                      Touch
                      PSRAM
                      QMI8658
                      PCF85063A
                      LVGL
                      examples
                      pico_stdlib
                      hardware_pio 
                      hardware_pwm 
                      hardware_dma
                      hardware_spi
                      hardware_i2c
                      hardware_gpio
                      hardware_adc
)

# Add the standard include files to the build
target_include_directories(RP2350-Touch-AMOLED-1.75-LVGL PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(RP2350-Touch-AMOLED-1.75-LVGL)

