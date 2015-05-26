/*
  SendDataToGoogleSpreadsheet

  Demonstrates appending a row of data to a Google spreadsheet from the Arduino Yun 
  using the Temboo Arduino Yun SDK.  

  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information

/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

const String GOOGLE_CLIENT_ID = "800673781747-oho2ba1qvjegkk9ia2qs2ebt975om891.apps.googleusercontent.com";
const String GOOGLE_CLIENT_SECRET = "H3PqNp2-Q34B_9rTXgz6f3HL";
const String GOOGLE_REFRESH_TOKEN = "1/cGMZ_KHYleWr0Fuz_CMawR5L9Hwf4eOWcwybP3BiW9Y";

// the title of the spreadsheet you want to send data to
// (Note that this must actually be the title of a Google spreadsheet
// that exists in your Google Drive/Docs account, and is configured
// as described above.)
const String SPREADSHEET_TITLE = "YUN";

int numRuns = 1;   // execution count, so this doesn't run forever
int maxRuns = 100;   // the max number of times the Google Spreadsheet Choreo should run

void setup() {
  
  // for debugging, wait until a serial console is connected
  Serial.begin(9600);
  delay(4000);
  while(!Serial);

  Serial.print("Initializing the bridge... ");
  Bridge.begin();
  Serial.println("Done!\n");
}

void loop()
{

  // while we haven't reached the max number of runs...
  if (numRuns <= maxRuns) {

    Serial.println("Running AppendRow - Run #" + String(numRuns++));

    // get the number of milliseconds this sketch has been running
    unsigned long now = millis();
    
    Serial.println("Getting sensor value...");

    // get the value we want to append to our spreadsheet
    unsigned long sensorValue = getSensorValue();

    Serial.println("Appending value to spreadsheet...");

    // we need a Process object to send a Choreo request to Temboo
    TembooChoreo AppendRowChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked and repopulated with
    // appropriate arguments each time its run() method is called.
    AppendRowChoreo.begin();
    
    // set Temboo account credentials
    AppendRowChoreo.setAccountName("pablozamora");
    AppendRowChoreo.setAppKeyName("pruebatemboo");
    AppendRowChoreo.setAppKey("4261d4f75a3f462783984cd528723a32");
    
    // identify the Temboo Library choreo to run (Google > Spreadsheets > AppendRow)
    AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
    
    // set the required Choreo inputs
    // see https://www.temboo.com/library/Library/Google/Spreadsheets/AppendRow/ 
    // for complete details about the inputs for this Choreo
    
    // your Google Client ID
    AppendRowChoreo.addInput("ClientID", GOOGLE_CLIENT_ID);

    // your Google Client Secret
    AppendRowChoreo.addInput("ClientSecret", GOOGLE_CLIENT_SECRET);

    // your Google Refresh Token
    AppendRowChoreo.addInput("RefreshToken", GOOGLE_REFRESH_TOKEN);

    // the title of the spreadsheet you want to append to
    AppendRowChoreo.addInput("SpreadsheetTitle", SPREADSHEET_TITLE);

    // convert the time and sensor values to a comma separated string
    String rowData(now);
    rowData += ",";
    rowData += sensorValue;

    // add the RowData input item
    AppendRowChoreo.addInput("RowData", rowData);

    // run the Choreo and wait for the results
    // The return code (returnCode) will indicate success or failure 
    unsigned int returnCode = AppendRowChoreo.run();

    // return code of zero (0) means success
    if (returnCode == 0) {
      Serial.println("Success! Appended " + rowData);
      Serial.println("");
    } else {
      // return code of anything other than zero means failure  
      // read and display any error messages
      while (AppendRowChoreo.available()) {
        char c = AppendRowChoreo.read();
        Serial.print(c);
      }
    }

    AppendRowChoreo.close();
  }

  Serial.println("Waiting...");
  delay(5000); // wait 5 seconds between AppendRow calls
}

// this function simulates reading the value of a sensor 
unsigned long getSensorValue() {
  return analogRead(A0);
}

