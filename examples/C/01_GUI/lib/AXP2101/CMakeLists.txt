# Search for all source files in the current directory
# and save the names to the DIR_AXP2101_SRCS variable
aux_source_directory(. DIR_AXP2101_SRCS)

include_directories(../Config)

# Adds a library target 
add_library(AXP2101 ${DIR_AXP2101_SRCS})
target_link_libraries(AXP2101 PUBLIC Config pico_stdlib hardware_gpio)