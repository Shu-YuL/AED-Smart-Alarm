set(srcs "main.c" "monitoring_device.c" "wifi_functions.c")
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ca.pem client.crt client.key)