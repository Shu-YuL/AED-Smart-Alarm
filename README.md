# AED Smart Alarm 
--updated Feb 3, 2023 by Shu-Yu Lin

## Introduction

Our project aims to improve upon the current AED monitoring system used by the University of Alberta Emergency Management Team (UAEMT) and University of Alberta Protective Services (UAPS). The current system has issues with delays in receiving alerts and the need for frequent re-authentication. Our new system addresses these issues and provides more advanced notice for time-sensitive alerts, enabling UAPS to respond more quickly in case of a medical emergency.

## Method of Project Approach

Our project consists of three main components. The first component is the alarm, a monitoring device installed inside each AED cabinet and used to detect if the cabinet door opens. The second component is a base station unit installed inside the UAPS office used notify people in the office if any of the alarm monitoring devices inside the AED cabinet are triggered. The third component is a Google Apps Script managed Google Sheets, it processes HTTP requests sent from the base station and the monitoring devices and sends an email to UAPS if any of the monitoring devices are triggered. 

### Monitoring Device
- Every monitoring device is powered by a 9V battery. 
- A door switch sensor is used to detect any motion of the cabinet door. 
- The activation of the switch sends a digital signal to wake up and trigger the device. 
- The development board used is ESP32-DevKitC V4. 
- When the MCU is active, it will try to connect to campus WiFi and sends its MAC address to the Google Sheet through an HTTP POST request.
- The monitoring device will go back to sleep after it receives an acknowledgement flag from the Google Sheet.

### Base Station
- The base station is powered by a wall outlet power supply that will supply 5V. 
- The MCU used for the base station is ESP32-WROOM-32E.
- An 2004 LCD display is mounted on the base station and connected to the MCU via the I²C interface. 
- LED bulbs are used and controlled via the MCU's digital output signal. Similarly, a piezoelectric buzzer is also used and controlled via PWM signals from the MCU.
- The base station is to be connected to campus Wifi and pull data from the Google Sheet every 10 seconds through sending an HTTP GET request. It receives an empty message when no monitoring device is triggered. If any of the monitoring devices are triggered, it receives a string showing the location of this monitoring device and then activates buzzer, LED, and also displays alert message on the LCD display.
- Users can interact with the base station with onboard pushbuttons. One pushbutton labeled "Clear" is used to dismiss an alert event.

### Google Sheet
- ...

The project firmware is developed using ESP-IDF. The Google Sheet is managed using Google Apps Script.

## Usage of this Repository
This repository contains all the code, files such as the PCB layout and 3D models, and documentation for the AED Smart Alarm project. Please refer to the documentation for instructions on how to set up and use the system. It includes details of the components used, such as the microcontroller, camera module, OLED display, LED bulb and piezoelectric buzzer.
