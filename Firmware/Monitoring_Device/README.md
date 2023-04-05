# Monitoring Device

-- updated March 9, 2023 by Shu-Yu Lin

-- updated March 17, 2023 by Shu-Yu Lin

-- updated April 4, 2023 by Shu-Yu Lin

## Development Progress

### What's ready

- Wifi
- HTTP Client
  - Door switch (currently using a button to simulate triggering behavior) GPIO interrupt (Post device MAC addr to Google Sheet)
- Deep Sleep

### What's left

- None

## How to use

Pull the **```Monitoring_Device```** folder to your local folder and copy and paste these files to your local project folder and build from there.

In **```wifi_include.h```**, the first line ``` #define ... ```, please enter ``` HOME ``` if you are off campus or using on campus **```DevNet```** WiFi, ``` UWS ``` if you are on campus.

Remember to enter your WiFi **```SSID```** and **```Password```** in **```wifi_include.h```** for home internet. Enter your **```CCID```** and **```Password```** in **```UWS_include.h```** for campus internet.

**Only pushes the same files back to the Repository** as other folders are associated with your own PC environment (ex: your ESP-IDF path) which is different from other people's environment.

## Pinout

(ESP32-DevkitC-V4)

### Door Switch (PIN 25)

![Button image](https://www.projecthub.in/wp-content/uploads/2019/12/pushbutton_diagram.png)

Lead 1 - PIN 25 INPUT, PULLDOWN, Low level wake up

Lead 2 - 3.3V
