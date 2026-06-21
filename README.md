# Recv router CSI example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The device triggers the router to send packets through the Ping command to obtain the CSI data between the device and the router

## How to use example
Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3/ESP32-S3/ESP32-C5/ESP32-C6/ESP32-C61 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

### Configure the project

```
idf.py menuconfig
```


### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


