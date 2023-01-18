# AED Smart Alarm --updated Jan 17, 2023 by Shu-Yu Lin

## Introduction

Our project aims to improve upon the current AED monitoring system used by the University of Alberta Emergency Management Team (UAEMT) and University of Alberta Protective Services (UAPS). The current system has issues with delays in receiving alerts and the need for frequent re-authentication. Our new system addresses these issues and provides more advanced notice for time-sensitive alerts, enabling UAPS to respond more quickly in case of a medical emergency.

## Method of Project Approach

Our project consists of two main components. The first component is the alarm, a monitoring device installed inside each AED cabinet and used to detect if the cabinet door opens. The second component is a base station unit installed inside the UAPS office used to monitor and notify them if any of the alarm monitoring devices inside the AED cabinet are triggered. 

### Monitoring Device
- A 9V battery will power each monitoring device. 
- A door switch sensor will be used to detect any motion of the cabinet door. 
- The activation of the switch will send a digital signal to the MCU. 
- The MCU used is ESP32-CAM which includes an integrated camera module (OV2640) connected through an onboard FPC connector. 
- When the MCU is active, the camera module will attempt to capture a picture of the individual that opened the cabinet door.
- The monitoring device will communicate with the base station through HTTP requests and sends the image along with the location details to the base station.

### Base Station
- The base station will be powered by a wall outlet power supply that will supply 5V. 
- An HTTP server will be hosted by the base station and the monitoring device communicates with the base station through HTTP Requests.
- The web server provides an user interface(UI) using HTML, CSS, Javascript, and can be access through browser.
- Users can interact with the base station with an onboard pushbutton. 
- When a monitoring device is triggered, it will send information, including the image taken to the base station. 
- If the user pushes the onboard push button, the base station will email this picture to UAPS or UAEMT. 
- The base station will delete the photo if the button is not pressed within 5 minutes.
- An OLED display will be mounted on the base station and connected to the MCU via the I²C interface. 
- An LED bulb will be used and controlled via the MCU's digital output signal. Similarly, a piezoelectric buzzer will also be used and controlled via PWM signals from the MCU.

Both the monitoring device and the base station will be connected to the internet through Wi-Fi.

The project firmware is developed using ESP-IDF.

## Usage of this Repository
This repository contains all the code, files such as the PCB layout and 3D models, and documentation for the AED Smart Alarm project. Please refer to the documentation for instructions on how to set up and use the system. It includes details of the components used, such as the microcontroller, camera module, OLED display, LED bulb and piezoelectric buzzer.
