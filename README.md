# ESP32 Covid - 19 project

This project belongs to Msc in Iot from UCM. The aim of it is getting measures of CO2 from the air to estimate the spread of Covid - 19. In addition it measures the temperature of the rrom as well.


## How to use example

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi and connection to internet.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu.
* Configure the broker for MQTT.
* By default, operating hours go from 10pm to 8am but you can also modify them
* In order to use the HTPPS server, you will have to able the following option: Component config -> ESP HTTPS server -> Enable ESP HTTPS server component
* When using Make build system, set `Default serial port` under `Serial flasher config`.

### Node RED

The file "flows.json" contains the logic for the Node RED application.

### Thingsboard

https://demo.thingsboard.io/dashboard/191c5760-67c8-11eb-839c-db4d34f02f5e?publicId=f5a21cc0-67d1-11eb-839c-db4d34f02f5e 

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

