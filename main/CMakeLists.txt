idf_component_register(SRCS "wireless_Drive.cpp" "twai_test_main.c" "webserver_service.cpp" "wifi_service.cpp" "controls.cpp"
                     "twai_service.cpp" "main.cpp"
                     "events.cpp" "led.cpp"
                     "imu_service.cpp"
                     "encoder_service.cpp"
                    
                    INCLUDE_DIRS "include"
                    )
spiffs_create_partition_image(storage ../partition FLASH_IN_PROJECT)
