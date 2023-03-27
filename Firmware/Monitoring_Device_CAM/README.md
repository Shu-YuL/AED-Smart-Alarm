# Monitoring Device with Camera

-- updated March 9, 2023 by Shu-Yu Lin
-- updated March 12, 2023 by Shu-Yu Lin
-- updated March 17, 2023 by Shu-Yu Lin

## Development Progress

### What's ready

- Wifi
- HTTP Client
  - Door switch (currently using a button to simulate triggering behavior) GPIO interrupt (Post device MAC addr to Google Sheet)
- Camera Video Stream Server

### What's left

- Devnet Access testing

## How to use

Pull the **```Monitoring_Device```** folder to your local folder and copy and paste these files to your local project folder and build from there.

Remember to enter your WiFi **```SSID```** and **```Password```** in **```wifi_include.h```** for home internet.

**Only pushes the same files back to the Repository** as other folders are associated with your own PC environment (ex: your ESP-IDF path) which is different from other people's environment.

## Pinout

(ESP32-CAM)

### Door Switch (Currently PIN 18)

![Button image](https://www.projecthub.in/wp-content/uploads/2019/12/pushbutton_diagram.png)

Lead 1 - PIN 18 INPUT, PULLDOWN, POS EDGE TRIGGER

Lead 2 - 3.3V
