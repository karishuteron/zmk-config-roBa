zephyr_library()
zephyr_library_include_directories(include)
zephyr_library_sources(src/sensor_delay_block.c)
add_subdirectory(behavior-tap-block)