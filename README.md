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

### Configuring the Example
To configure the example to use Wi-Fi, Ethernet, Thread or all connections, open the project configuration menu (idf.py menuconfig) and navigate to "Example Connection Configuration" menu. Select either "Wi-Fi" or "Ethernet" or "Thread" or all in the "Connect using" choice.

When connecting using Wi-Fi, enter SSID and password of your Wi-Fi access point into the corresponding fields. If connecting to an open Wi-Fi network, keep the password field empty.

When connecting using Ethernet, set up PHY type and configuration in the provided fields. If using Ethernet for the first time, it is recommended to start with the Ethernet example readme, which contains instructions for connecting and configuring the PHY. Once Ethernet example obtains IP address successfully, proceed to the protocols example and set the same configuration options.

When connecting using Thread, enter the Thread network parameters in Component config-->OpenThread-->Thread Core Features-->Thread Operational Dataset. For instructions on starting or joining a Thread network, refer to the ot_cli example. If the example requires to access an IPv4 network, a Thread Border Router with NAT64 (Network Address Translation IPv6-IPv4) support is needed. You can follow this guide to set up the ESP Thread Border Router for testing.




### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

## Referances:
- https://github.com/espressif/esp-csi
- https://github.com/espressif/esp-idf
