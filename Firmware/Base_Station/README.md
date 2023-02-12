# Base Station

## Development Progress
### What's ready
- Wifi
- HTTP Client
  - 5 sec recurring timer interrupt (read data from Google Sheet)
  - Clear button GPIO interrupt (remove row 2 in *Triggered_devices* sheet, same as clearing an event)
### What's left
- LCD (debug)
- Peripheral GPIO features (LED, buzzer, etc.)

## How to use
Pull the **Base_Station** folder to your local folder and copy and paste these files to your local project folder and build from there.

Remember to enter your WiFi **SSID** and **Password** in **wifi_include.h**

**Only pushes the same files back to the Repository** as other folders are associated with your own PC enviornment (ex: your ESP-IDF path) which is different from other perople's enviornment.

## Pinout
(ESP32-DevkitC-V4)
### LCD
PIN 21 - SDA
PIN 22 - SCL
5V     - VCC
GND    - GND

### Clear Button
![Button image](https://www.projecthub.in/wp-content/uploads/2019/12/pushbutton_diagram.png)

Lead 1 - PIN 19 INPUT, PULLDOWN, POS EDGE TRIGGER
Lead 2 - 3.3V
