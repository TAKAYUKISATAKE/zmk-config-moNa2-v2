# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  target_sources(app PRIVATE src/behavior_host_key.c)
  # Adapt existing Studio bindings without rewriting anything in settings.
  zephyr_ld_options(-Wl,--wrap=zmk_behavior_invoke_binding)
endif()
