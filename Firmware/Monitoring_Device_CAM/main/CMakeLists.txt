set(srcs "main.c" "monitoring_device.c" "wifi_functions.c" "camera_functions.c")
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    )

                    target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)