## **diy-co2-monitor**
# DIY, low cost esp-platform based CO2 monitor ;-) 

> **This project is under development!!!** \
> **Current release should work for real-time CO2 monitoring 'out-of-the-box'.**

This repository contains the firmware source code for a low-cost CO2 monitor built using [platform.io](https://docs.platformio.org/en/latest/what-is-platformio.html) framework, and standard ESP modules such Wemos Lolin32 and clones.

Current hardware setup, includes support for (total cost about 22€):
- Wemos Lolin32 OLED Board and clones (ESP-WROOM-32).
- MH-Z19B CO2 Sensor (0-5000ppm +/-50ppm range).

![prototype picture](./img/prototype_picture.png)

## **Sensor wiring**
As the ESP module is able to run supplied from standard USB Smartphone charger, it is only need to wire 4 cables between ESP module and MH-Z19B Sensor:

ESP module | MH-Z19B Sensor
-----------|----------------
5V         | Vin
GND        | GND
PIN 25     | Tx
PIN 26     | Rx

## **Compile and run**
- Install [Visual Studio Code](https://code.visualstudio.com/Download)
- Install [platform.io plugin](https://platformio.org/install/ide?install=vscode)
- Clone (or download) this [repository](https://github.com/oskrs111/diy-co2-monitor/archive/main.zip)
- Open folder with Visual Studio Code.
- Wait to platform.io plugin to load.
- Do Upload (note that you have to press the Boot button, on the ESP module, until the uploading starts).


![CC BY SA](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)

