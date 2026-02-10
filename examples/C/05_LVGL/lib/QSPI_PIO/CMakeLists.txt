# Search for all source files in the current directory
# and save the names to the DIR_QSPI_PIO_SRCS variable
aux_source_directory(. DIR_QSPI_PIO_SRCS)

# Adds a library target 
add_library(QSPI_PIO ${DIR_QSPI_PIO_SRCS})
target_link_libraries(QSPI_PIO PUBLIC pico_stdlib hardware_pio hardware_gpio)