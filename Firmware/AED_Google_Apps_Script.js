/* Group: 16 AED Smart Alarm - Web Server
 * Module: main
 * Description: This Google Apps Script code is a functional web server backend for our AED smart alarm system that handles requests from a base station and monitoring devices. The script logs events and sends notifications to the client when the alarm is triggered or cleared.
 * Authors: Shu-Yu Lin
 * References:  - Google Workspace Apps Script Guide
 *                  - https://developers.google.com/apps-script/guides/web
 * Revision Information: V.14.2
 * Date: 5/April/2023
 * Copyright: N/A
 * Functions: - doGet(e)
              - sendTelegramMessage(text)
              - sendToDiscord(message)
              - Alarm_Count(mac)
              - Check_BatteryService_Date()
              - clearFirstNonEmptyCell()
*/

/* opens the Google Sheet and retrieves the three sheets within the workbook */
var sheet_id = "XXX"; // Google Sheet ID
var ss = SpreadsheetApp.openById(sheet_id);
var paired_sheet = ss.getSheetByName("Paired_devices");
var triggered_sheet = ss.getSheetByName("Triggered_devices");
var log_sheet = ss.getSheetByName("Event_Log");

/* set up recipient email addresses */
var email_addrs_range = paired_sheet.getRange(2,5,5,1); // get cell range starts from row 2, column 5, range is 5 rows and 1 column
var email_addrs = email_addrs_range.getValues(); // get cell values from this range. getValues returns a 2D array
var recipient_emails = [];
for (var i = 0; i < email_addrs.length; i++) {
  recipient_emails.push(email_addrs[i][0]); // extract value from the 2D array and form a 1D array
}
recipient_emails = recipient_emails.join(","); // join the array values into a single string, values separated by ","

var max_bat_count = 50; // the maximum number for the alarm to go off in one battery life
var max_bat_service_time = 15; // the maximum number of days the battery can last

var timerId = 0; // initialize video URL auto delete timer ID holder

/* function that deal with HTTP GET requests */
function doGet(e)
{
  var pairedLastRow = paired_sheet.getLastRow();
  var triggeredLastRow = triggered_sheet.getLastRow();
  var logLastRow = log_sheet.getLastRow();
  /* get current date and time */
  var date = new Date();

  /***** For Base Station *****/
  var read = e.parameter.read; // set key value = "read" to read data from the Google sheet
  var clear = e.parameter.clear; // set key value = "clear" to clear row 2 in triggered_sheet (clear the event)
  if (read !== undefined)
  {
    if (clear !== undefined)
    {
      var MAC = triggered_sheet.getRange(2,1).getValue();
      var location = triggered_sheet.getRange(2,2).getValue();
      /* search for a matching MAC in the "Event_Log" sheet. (software de-bouncer) */ 
      for (var i = logLastRow; i > 1; i--) 
      {
        if (log_sheet.getRange(i, 1).getValue() == MAC) 
        {
          var lastTriggerTime = log_sheet.getRange(i, 3).getValue(); // retrieves the time this device last triggered
          var timeDiffInSecs = Math.floor((date.getTime() - lastTriggerTime.getTime()) / 1000); // get the time different from the last time this device triggered
          break;
        }
        else
        {
          return ContentService.createTextOutput('Sheet Empty');
        }
      }
      /* if last time a request was made for this device more than 5 secs ago, the request will get processed, otherwise, discard repetitive requests */
      if (timeDiffInSecs > 5)
      {
        triggered_sheet.deleteRow(2);  // if Clear button is pressed, delete row 2
        log_sheet.appendRow([MAC,location,date,"Event Cleared by Base Station"]); // log the event
        /* --------- Send Email ----------- */
        if (recipient_emails != ",,,,")
        {
          MailApp.sendEmail({
          to: recipient_emails,
          cc: "",
          subject: "AED Smart Alarm - Event Cleared",
          body: `The following Event was cleared by the Base Station:\n\nLocation: ${location} \nTime: ${date} \nDevice MAC: ${MAC} \n\nIf this was made by a mistake you can check the record on the Google Sheet.`
          })
        }
        /* send notification to Telegram Channel */
        sendTelegramMessage(`The following Event was cleared by the Base Station:\n\nLocation: ${location} \nTime: ${date} \nDevice MAC: ${MAC} \n\nIf this was made by a mistake you can check the record on the Google Sheet.`);
        /* send notification to Discord Channel */
        sendToDiscord(`\`\`\`The following Event was cleared by the Base Station:\n\nLocation: ${location} \nTime: ${date} \nDevice MAC: ${MAC} \n\nIf this was made by a mistake you can check the record on the Google Sheet.\`\`\``);
      }
      return ContentService.createTextOutput('Cleared');
    }
    return ContentService.createTextOutput(triggered_sheet.getRange('B2').getValue()); // send the location value to Base Station
  }

  /***** For Monitoring Devices *****/
  /* retrieves data from HTTP request */
  var MAC = String(e.parameter.MAC);
  var IP = String(e.parameter.IP);

  var bat_count = 0;

  var Match_flag = 0;
  var Duplicate_flag = 0;
  var timeDiffInSecs = 0;

  /* search for a matching MAC in the "Paired_devices" sheet. */ 
  for (var i = 2; i <= pairedLastRow; i++) 
  {
    /* If a match is found, it retrieves the corresponding location from the paired sheet and saves it to the triggered sheet */ 
    if (paired_sheet.getRange(i, 1).getValue() == MAC) 
    {
      var location = paired_sheet.getRange(i, 2).getValue();
      Match_flag = 1; // flag indicates a match is found

      /* search for a matching MAC in the "Event_Log" sheet. */ 
      for (var i = logLastRow; i > 1; i--) 
      {
        if (log_sheet.getRange(i, 1).getValue() == MAC) 
        {
          var lastTriggerTime = log_sheet.getRange(i, 3).getValue(); // retrieves the time this device last triggered
          timeDiffInSecs = Math.floor((date.getTime() - lastTriggerTime.getTime()) / 1000); // get the time different from the last time this device triggered
          break;
        }
      }
      /* check for duplicate request */
      for (var i = triggeredLastRow; i > 1; i--) 
      {
        /* If a match is found, meaning duplicate request */ 
        if (triggered_sheet.getRange(i, 1).getValue() == MAC) 
        {
          Duplicate_flag = 1; // flag indicates duplicate request occurred
          break;
        }
      }
      break;
    }
  }
  
  /* if last time a request was made for this device more than 5 secs ago, the request will get processed, otherwise, discard repetitive requests */
  /* if duplicate request occurred, discard the request */
  if (((timeDiffInSecs > 5)||(timeDiffInSecs == 0))&&(Duplicate_flag == 0))
  {
    /* If Match flag set */
    if (Match_flag)
    {
      bat_count = Alarm_Count(MAC); // update device alarm count value

      if (IP !== "undefined") // for ESP32-CAM
      {
        triggered_sheet.appendRow([MAC,location,`http://${IP}/`]); // append new row in triggered_sheet
        // Create a timer to run the clearFirstNonEmptyCell function after 5 minutes
        var timer = ScriptApp.newTrigger("clearFirstNonEmptyCell")
          .timeBased()
          .after(5 * 60 * 1000) // after 5 mins
          .create();

        // Store timer ID in global variable
        timerId = timer.getUniqueId();

        if (bat_count == max_bat_count) // if reaches max number of count, send out battery service notice
        {
          log_sheet.appendRow([MAC,location,date,"Alarm Triggered & Battery Service (Reached max number of counts)"]); // log the event
          var notif_body = `***Alarm Triggered***\nLocation: ${location} \nTime: ${date} \nDevice MAC: ${MAC} \nVideo URL: http://${IP}/ \n\n**Warning**\nPotential low battery. Please check the battery for this device`;
        }
        else // otherwise, send out normal notice
        {
          log_sheet.appendRow([MAC,location,date,"Alarm Triggered"]); // log the event
          var notif_body = `***Alarm Triggered***\nLocation: ${location} \nTime: ${date} \nDevice MAC: ${MAC}\nVideo URL: http://${IP}/ `;
        }
      }
      else // for ESP32-DevKitC
      {
        triggered_sheet.appendRow([MAC,location]); // append new row in triggered_sheet
        if (bat_count == max_bat_count) // if reaches max number of count, send out battery service notice
        {
          log_sheet.appendRow([MAC,location,date,"Alarm Triggered & Battery Service (Reached max number of counts)"]); // log the event
          var notif_body = `***Alarm Triggered***\nLocation: ${location} \nTime: ${date} \nDevice MAC: ${MAC} \n\n**Warning**\nPotential low battery. Please check the battery for this device`;
        }
        else // otherwise, send out normal notice
        {
          log_sheet.appendRow([MAC,location,date,"Alarm Triggered"]); // log the event
          var notif_body = `***Alarm Triggered***\nLocation: ${location} \nTime: ${date} \nDevice MAC: ${MAC}`;
        }
      }

      /* --------- Send Email ----------- */
      if (recipient_emails != ",,,,")
      {
        MailApp.sendEmail({
        to: recipient_emails,
        cc: "",
        subject: "AED Smart Alarm",
        body: notif_body
        })
      }
      /* send notification to Telegram Channel */
      sendTelegramMessage(notif_body);
      /* send notification to Discord Channel */
      sendToDiscord(`\`\`\`${notif_body}\`\`\``);

      /* return acknowledgement (event received) to MD */
      return ContentService.createTextOutput('Accepted');
    }
    /* If no match, add the MAC to a new row in the "Paired_devices" sheet (Pairing a new device) */
    else
    {
      log_sheet.appendRow([MAC,location,date,"Device Paired"]); // log the event

      paired_sheet.appendRow([MAC,,bat_count,date]);

      /* return acknowledgement (pairing done) to MD */
      return ContentService.createTextOutput('Paired');
    }
  }
  /* return acknowledgement (event received) to MD */
  return ContentService.createTextOutput('Error');
}

/* send notification to Telegram Channel */
function sendTelegramMessage(text) {
  var botToken = "XXX"; // the Telegram bot's token that we created
  var chatId = "XXX"; // the Telegram channel ID that we created
  var url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatId + "&text=" + encodeURIComponent(text);
  var response = UrlFetchApp.fetch(url); // performs HTTP request to send out the message
  return response;
}

/* send notification to Discord Channel */
function sendToDiscord(message) {
  /* Discord Channel Webhook url */
  const url = "https://discord.com/api/webhooks/XXX"
  const formTEXT = {
    content: message
  }
  const options = {
    'method' : 'post', // http post method
    'payload' : formTEXT
  }
  const res = UrlFetchApp.fetch(url,options); // performs HTTP request to send out the message
}

/* This function takes in mac address and update the corresponding device's alarm count every time the device is triggered, and return the count value to the calling function */
function Alarm_Count(mac)
{
  var lastRow = paired_sheet.getLastRow(); // get location of the last row
  var currentDate = new Date(); // get current date and time
  var counter = 0; // initialise counter

  for (var i = 2; i <= lastRow; i++) { // Start from row 2 to avoid the header row
    var MAC = paired_sheet.getRange(i, 1).getValue();
    var currentCount = paired_sheet.getRange(i, 3).getValue();
    if (MAC == mac) { // Check if the MAC address in column A matches the one sent in the request
      paired_sheet.getRange(i, 3).setValue(currentCount+1);
      counter = currentCount+1;
      if (counter == max_bat_count) // if counter reaches max number
      {
        paired_sheet.getRange(i, 3).setValue(0); // reset cell value
        paired_sheet.getRange(i, 4).setValue(currentDate); // update battery service date and time
      }
      break;
    }
  }
  return counter;
}

/* This function performs daily automatically to check if any device hasn't replace its battery for more than the max number of days a battery can last */
function Check_BatteryService_Date()
{
  var lastRow = paired_sheet.getLastRow();
  var currentDate = new Date();

  for (var i = 2; i <= lastRow; i++) {
    var MAC = paired_sheet.getRange(i, 1).getValue();
    var location = paired_sheet.getRange(i, 2).getValue();
    var dateInColumnD = paired_sheet.getRange(i, 4).getValue();
    if (dateInColumnD !== "") {
      var timeDiffInDays = Math.floor((currentDate.getTime() - dateInColumnD.getTime()) / (1000*3600*24)); // /1000 to convert msec to sec, /3600 to convert sec to hours, /24 to convert hours to days

      if (timeDiffInDays > max_bat_service_time) { // if time difference reaches max number
        paired_sheet.getRange(i, 3).setValue(0); // reset cell value
        paired_sheet.getRange(i, 4).setValue(currentDate); // update battery service date and time

        log_sheet.appendRow([MAC,location,currentDate,"Battery Service (Reached max number of days)"]); // log the event

        /* --------- Send Email ----------- */
        if (recipient_emails != ",,,,")
        {
          MailApp.sendEmail({
          to: recipient_emails,
          cc: "",
          subject: "AED Smart Alarm - Battery Check Reminder",
          body: `The battery of the following device has not been replaced for a month, please pay attention to check the power.\n\nLocation: ${location} \nTime: ${currentDate} \nDevice MAC: ${MAC}\n\nPlease note that the timer on this device has now been reset and you will receive another reminder when the timer reaches one month again.` //note the back-ticks ` ; they are not single quotation marks
          })
        }

        /* send notification to Telegram Channel */
        sendTelegramMessage(`The battery of the following device has not been replaced for 15 days, please pay attention to check the capacity.\n\nLocation: ${location} \nTime: ${currentDate} \nDevice MAC: ${MAC}\n\nPlease note that the timer on this device has now been reset and you will receive another reminder when the timer reaches 15 days again.`);
        /* send notification to Discord Channel */
        sendToDiscord(`\`\`\`The battery of the following device has not been replaced for 15 days, please pay attention to check the capacity.\n\nLocation: ${location} \nTime: ${currentDate} \nDevice MAC: ${MAC}\n\nPlease note that the timer on this device has now been reset and you will receive another reminder when the timer reaches 15 days again.\`\`\``);
      }
    }
  }
}

/* This function delete the video streaming URL from the sheet */
function clearFirstNonEmptyCell() {
  var lastRow = triggered_sheet.getLastRow();

  // Loop through each row in column C, starting from row 2
  for (var i = 2; i <= lastRow; i++) {
    var dateInColumnC = triggered_sheet.getRange(i, 3).getValue();
    if (dateInColumnC != "") {
      // Clear the first non-empty cell in column C after row 1
      triggered_sheet.getRange(i, 3).clearContent();
      break;
    }
  }

  // Delete timer
  ScriptApp.getProjectTriggers().forEach(function(trigger) {
    if (trigger.getUniqueId() == timerId) {
      ScriptApp.deleteTrigger(trigger);
    }
  });
}