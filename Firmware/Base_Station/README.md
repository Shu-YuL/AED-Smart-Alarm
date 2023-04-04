# Base Station

-- updated March 1, 2023 by Shu-Yu Lin
-- updated March 17, 2023 by Shu-Yu Lin
-- updated April 4, 2023 by Mohammad Kamal
## Development Progress

### What's ready

- Wifi
- HTTP Client
  - 5 sec recurring timer interrupt (read data from Google Sheet)
  - Clear button GPIO interrupt (remove row 2 in *Triggered_devices* sheet, same as clearing an event)
- LCD Screen
- Buzzer
- Alarm LED

## How to use

Pull the **```Base_Station```** folder to your local folder and copy and paste these files to your local project folder and build from there.

In **```wifi_include.h```**, the first line ``` #define ... ```, please enter ``` HOME ``` if you are off campus or using on campus DevNet WiFi, ``` UWS ``` if you are on campus.

Remember to enter your WiFi **```SSID```** and **```Password```** in **```wifi_include.h```** for home internet. Enter your **```CCID```** and **```Password```** in **```UWS_include.h```** for campus internet.

**Only pushes the same files back to the Repository** as other folders are associated with your own PC environment (ex: your ESP-IDF path) which is different from other people's enviornment.

## Pinout

(ESP32-DevkitC-V4)

### LCD

PIN 18 - SDA

PIN 19 - SCL

5V     - VCC

GND    - GND

### Clear Button

![Button image](https://www.projecthub.in/wp-content/uploads/2019/12/pushbutton_diagram.png)

Lead 1 - PIN 22 INPUT, PULLDOWN, POS EDGE TRIGGER

Lead 2 - 3.3V

### Buzzer control Button

Lead 1 - PIN 21 INPUT, PULLDOWN, POS EDGE TRIGGER

Lead 2 - 3.3V
