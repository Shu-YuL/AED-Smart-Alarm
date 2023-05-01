# AED Smart Alarm

--updated Jan 17, 2023 by Shu-Yu Lin

--updated Feb 14, 2023 by Shu-Yu Lin

--updated Mar 14, 2023 by Shu-Yu Lin

--updated Mar 27, 2023 by Shu-Yu Lin

--updated Apr 4, 2023 by Shu-Yu Lin

--updated Apr 30, 2023 by Shu-Yu Lin

----------------------------------------------------------------

## Demo Video

[![Demo Video](https://lh3.googleusercontent.com/fife/APg5EOZy0qmSeqRbGbYMGeu49TxH5lmIOj3HhFyADLr6qH4S8TAaIJzbBPQnfgN_s-cPjZGFoLVAi6fIpvv_eCJc7_dvCM5OVtgkbksEu1INZQIVRCa6F0s9MO7LJb4W_VmwPlCb04PSN6UpbPwQ0xZxmr8z40CDwzgnBYsCoBz82gxHBdwRRx3AK6PTTbE4swi1r4OC8VxJj83QZSaBq_axB13bFNfvSjfkM9lRGnLXVgempMBgW_JMlQ8JZupSTVby1mXDgkiXalZTfnvae9LtgSR2kD4ZaaPTBrLYbnSjJoOcwekloa4i9upgvwhmV6UuGPqelpdEVIKUb0zCZGPnMjvvsTw_2Po14uoSeGETeqKRDd6uW2YArJD77ucuMG6qNt2Gem0yAI9NxQhCjbwBNYsCyjXGaa2BgXaY-8XdDqX1Mx07VmGnCa4fYvGSmAh2E12dP3vBXci5WtxpB1mUBuGlYkXIpvd327HO3nS2LpaJxP5piBP93a9GQkfz6OSpcxceiy6YWZIpN3cmGjMtxCaoLY8ixNX4d9ixSc4fpJpaSr7I3dkZImk0GOwjqR3xibleN_GsKG0moCss992vfX2BKf0Jc9spBMjNV6-2EfTBknmBre45v9of6kBL2GSb3d6iRzKlJ31yBRbIjqMMEWdltXHwqPHpjVWfJihoVlNe3wC1QZjWhIwFi8nv6nqt33q25jnV4J3Cq5mtlX1iGLJC8Z48cVMwxL9PpYGNN1b59B_K5U_RwdsI4gLgNnz999uuzSIeae5vx8KiVRm7SkXUUmuWnSZf90uaKBqZnekPYPGBeCjfYeUKWx3TFvJ6D0dil_QGtLUs6PXrIOAVoQ4H-kTlWyh0TMBW9p63Tly5B2JojbUkzhm8_w84FCkqoo5ElOnNsiUPa9Vcq175EdBHcfhmW7sAL8E7n9_DCXVNkeGwU5FgteWTIfNH1ESyxW_-bmufeuZ6pC-AliJgboJIaW5ZRdk58-B8PeqJ1hRr_7eFB8cpDLPJBaY8Vsg7hvgu3LZmLCbm73OjMXCEUkPtxawnfU5iJiKNJTYGJsBCpuVoUwR9EyLYlWgCaQFeHb-f_gmBUNcvSdV69-jib-mk7-MQOCFCYeSE1sNLpaVtzJmPjppp7XQHR8aCpdl6hikow4ChZ8Y4b8jt7gqkqRCYXpGSENUku35SLy19q8GuZnQ4OqCBEZjhRtRddLTxCOrBvjnOxRmyqougyYPEXUmoWOx0TKGyY6Hjho5ErniS5xrw8hKajbjSjN2OwMH83XZyLO3kg3vtkb_LYo6-RBT3pzAC8Ps_SLnvr4ILTwVZnF5t3VGGtdLRFFjI0h4WdgBgli0Bb7BPlsqJt8sOZdLPsWhUT5cgD280GumtA7FQvgNvclBMwaH1jCS99OAF7J4bOnOJqg_cTY4eNW_orggrD6oAf2BFiCVVzpNTNuUOSClDvBvviMVxwNR2MIcZnvopvAHA721Vph1cjg38kVMJq1Zw_ECSs386fh8K1YeYqixkRFTugn3-30Zh77XmVHoywLTSx9y_oey-ck5fwWFaPlW8vO2vpDeKGk-cMl2PZALwcuiQga5bR0iar3lV_XzlYYY=w1920-h1080-k-pd)](https://drive.google.com/file/d/1JeJAB9AQrucV8XEORTZ0Mz8k3fBR3jVF/view?usp=sharing "Demo video")

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
