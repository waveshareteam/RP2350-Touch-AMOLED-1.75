
add_library(PSRAM INTERFACE)

target_sources(
    PSRAM
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/rp_pico_alloc.c
              ${CMAKE_CURRENT_LIST_DIR}/rp_pico_alloc.h
              ${CMAKE_CURRENT_LIST_DIR}/tlsf/tlsf.c
              ${CMAKE_CURRENT_LIST_DIR}/tlsf/tlsf.h
              ${CMAKE_CURRENT_LIST_DIR}/tlsf/tlsf_common.h
              ${CMAKE_CURRENT_LIST_DIR}/tlsf/tlsf_block_functions.h)

# Are we wrapping the built in alloc functions?
if (RP_PICO_ALLOC_WRAP)
    pico_wrap_function(PSRAM malloc)
    pico_wrap_function(PSRAM calloc)
    pico_wrap_function(PSRAM realloc)
    pico_wrap_function(PSRAM free)
endif ()

target_link_libraries(PSRAM INTERFACE pico_stdlib hardware_spi hardware_gpio hardware_flash pico_flash
                                              hardware_exception hardware_sync)
