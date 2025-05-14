# Source code for RESCUE Roller Operating system built atop the ESP-IDF and microROS.

## Requirements
ESP-IDF 
microROS

## Getting Started
Clone the repository
```
git submodule init
git submodule update
```
These commands pull third party repositories into your clone repository.

## Building
```mkdir build && cd build
source /path/to/idf/install/esp/esp-idf/export.sh
idf.py set-target esp32s3
idf.py menuconfig
```

microROS Settings 
- Set your micro-ROS Agent IP  
microROS Settings > Wifi Configuration
- Set your WiFi SSID and Password
```
idf.py build
idf.py flash
```



Resources:
https://www.freertos.org/Documentation/00-Overview
https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/get-started/index.html
