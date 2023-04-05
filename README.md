# AED Smart Alarm

--updated Jan 17, 2023 by Shu-Yu Lin

--updated Feb 14, 2023 by Shu-Yu Lin

--updated Mar 14, 2023 by Shu-Yu Lin

--updated Mar 27, 2023 by Shu-Yu Lin

--updated Apr 4, 2023 by Shu-Yu Lin

----------------------------------------------------------------

Alarm messaging app channel invite links:

[Telegram](https://t.me/+YDv_uXch1lFhM2Mx)

[Discord](https://discord.gg/8R3kqmZn9D)

## DevNet Access Password Record

(Need to register the device's MAC address first before granting access to DevNet)

| Password |
| ------------------|
| SvSAxkQGy4PC7D37 |

## Introduction

Our project aims to improve upon the current AED monitoring system used by the University of Alberta Emergency Management Team (UAEMT) and University of Alberta Protective Services (UAPS). The current system has issues with delays in receiving alerts and the need for frequent re-authentication. Our new system addresses these issues and provides more advanced notice for time-sensitive alerts, enabling UAPS to respond more quickly in case of a medical emergency.

## Method of Project Approach

Our project consists of three main components. The first component is the alarm, a monitoring device installed inside each AED cabinet and used to detect if the cabinet door opens. The second component is a base station unit installed inside the UAPS office used notify people in the office if any of the alarm monitoring devices inside the AED cabinet are triggered. The third component is a Google Apps Script managed Google Sheets, it processes HTTP requests sent from the base station and the monitoring devices and sends **an email, along	with Telegram and Discord channel announcement message** to UAPS if any of the monitoring devices are triggered.

### Monitoring Device

- Every monitoring device is powered by batteries.
- A door switch sensor is used to detect any motion of the cabinet door.
- The activation of the switch sends a digital signal to wake up and trigger the device.
- The development board used is ESP32-DevKitC V4.
- When the MCU is active, it will try to connect to campus WiFi and sends its MAC address to the Google Sheet through an HTTP GET request.
- The monitoring device will go back to sleep after it receives an acknowledgement from the Google Sheet.

### Base Station

- The base station is powered by a wall outlet power supply that will supply 5V.
- The MCU used for the base station is ESP32-WROOM-32E.
- An 2004 LCD display is mounted on the base station and connected to the MCU via the I²C interface.
- LED bulbs are used and controlled via the MCU's digital output signal. Similarly, a piezoelectric buzzer is also used and controlled via PWM signals from the MCU.
- The base station is to be connected to campus Wifi and pull data from the Google Sheet every 10 seconds through sending an HTTP GET request. It receives an empty message when no monitoring device is triggered. If any of the monitoring devices are triggered, it receives a string showing the location of this monitoring device and then activates buzzer, LED, and also displays alert message on the LCD display.
- Users can interact with the base station with onboard pushbuttons. One pushbutton labeled "Clear" is used to dismiss an alert event, while the other one is used to turn off the buzzer.

### Google Sheet

- Serves as a Web Server and online User Interface for the project
- Manages HTTP Requests and data sent from both the Base Station and the Monitoring Devices
- Provides an paired device list that can be managed by the user
- Provides an Event Log sheet that records all triggering events along with triggered date and time of each event
- Sends notifications through email, Telegram and Discord channel when an event is triggered or cleared
- Sends notifications through email, Telegram and Discord channel when a prospect low battery is detected

The project firmware is developed in C language using ESP-IDF. The Google Sheet is managed using Google Apps Script which the script is developed in JavaScript.

## Usage of this Repository

This repository contains all the code, files such as the hardware design project, PCB layout and 3D models. Please refer to the documentation for instructions on how to set up and use the system.
