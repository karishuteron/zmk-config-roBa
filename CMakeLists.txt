zephyr_include_directories(include)

if(CONFIG_SHIELD_ROBA_R)
  target_sources(app PRIVATE src/behavior_pmw3610_cpi.c)
endif()
