if(CONFIG_SHIELD_ROBA_R OR CONFIG_SHIELD_ROBA_L)
  target_sources(app PRIVATE src/behavior_pmw3610_cpi.c)
endif()
