idf_component_register(SRCS "wifi_functions.c" "server_functions.c" "main.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ca.pem client.crt client.key)
