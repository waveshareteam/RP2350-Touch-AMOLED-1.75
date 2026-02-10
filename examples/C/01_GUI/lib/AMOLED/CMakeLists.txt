# Search for all source files in the current directory
# and save the names to the DIR_AMOLED_SRCS variable
aux_source_directory(. DIR_AMOLED_SRCS)

include_directories(../Config)
include_directories(../QSPI_PIO)

# Adds a library target 
add_library(AMOLED ${DIR_AMOLED_SRCS})
target_link_libraries(AMOLED PUBLIC QSPI_PIO Config pico_stdlib hardware_gpio)