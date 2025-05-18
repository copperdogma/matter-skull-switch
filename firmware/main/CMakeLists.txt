idf_component_register(SRC_DIRS          "." "drivers"
                       INCLUDE_DIRS      "." "drivers/include"
                       PRIV_INCLUDE_DIRS "." 
                                       "drivers/include" 
                       REQUIRES espressif__esp_matter
                       )

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
