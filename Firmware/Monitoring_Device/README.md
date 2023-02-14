# Monitoring Device

## Development Progress

### What's ready

- Wifi
- HTTP Client
  - Door switch (currently using a button to simulate triggering behavioor) GPIO interrupt (Post device MAC addr to Google Sheet)

### What's left

- Auto sleep after complete a whole process
- Wake up on Door switch trigger
- Optimal power saving

## How to use

Pull the **Monitoring_Device** folder to your local folder and copy and paste these files to your local project folder and build from there.

Remember to enter your WiFi **SSID** and **Password** in **menuconfig**

VS Code: CTRL+SHIFT+P -> search "ESP-IDF: SDK Configuration Editor" -> look for "Example Configuration" -> type in SSID and Password -> Save -> Build & Flash)

**Only pushes the same files back to the Repository** as other folders are associated with your own PC enviornment (ex: your ESP-IDF path) which is different from other perople's enviornment.

## Pinout

(ESP32-DevkitC-V4)

### Clear Button (Currently PIN 18)

![Button image](https://www.projecthub.in/wp-content/uploads/2019/12/pushbutton_diagram.png)

Lead 1 - PIN 18 INPUT, PULLDOWN, POS EDGE TRIGGER

Lead 2 - 3.3V
