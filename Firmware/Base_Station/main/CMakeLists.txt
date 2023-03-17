idf_component_register(SRCS "Buzzer_functions.c" "base_station.c" "main.c" "HD44780.c" "wifi_functions.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ca.pem client.crt client.key)
