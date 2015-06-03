
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information

#define RainPin 5

//Rain Variables
bool RainHigh=false;
const float LowAmt=1.0; //when rain is low, takes this ml to trip
const float HiAmt=1.0;    //when rain is high, takes this ml to trip
float RainAccum=0.0;     //Rain accumulator since start of sample
float DiezMl=0.0;       //10 mL accumulator

//WindSpeed Variables
const int sensorPin = A0; //Defines the pin that the anemometer output is connected to
int sensorValue = 0; //Variable stores the value direct from the analog pin
float sensorVoltage = 0; //Variable that stores the voltage (in Volts) from the anemometer being sent to the analog pin
float windSpeed = 0; // Wind speed in meters per second (m/s)
float windSpeed2= 0; // Wind speed in kilometer per hour (km/h)
float voltageConversionConstant = .004882814; //This constant maps the value provided from the analog read function, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V
int sensorDelay = 1000; //Delay between sensor readings, measured in milliseconds (ms)

//Anemometer Technical Variables
//The following variables correspond to the anemometer.

float voltageMin = 1.0150; // Mininum output voltage from anemometer in V.
float windSpeedMin = 0; // Wind speed in meters/sec corresponding to minimum voltage
float voltageMax = 5; // Maximum output voltage from anemometer in V.
float windSpeedMax = 60; // Wind speed in meters/sec corresponding to maximum voltage


// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

const String GOOGLE_CLIENT_ID = "979674851330-cupbsll9jmk4g6rrog95s5gccslk6bso.apps.googleusercontent.com";
const String GOOGLE_CLIENT_SECRET = "be6ubxGEfZWEuSjDdQ7XAbZc";
const String GOOGLE_REFRESH_TOKEN = "1/d6EtVlijERCm9_VyqR0zQ1J7bfFLHvLrlFDuvEBozB0";

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
  
  if (digitalRead(RainPin)==HIGH)
    {
        RainHigh=true;
    }
   else
   {
        RainHigh=false;
    }
}

void loop()
{

  // while we haven't reached the max number of runs...
  if (numRuns <= maxRuns) {

    Serial.println("Running AppendRow - Run #" + String(numRuns++));

    // get the number of milliseconds this sketch has been running
    unsigned long now = millis();
    
    Serial.println("Getting sensor value...");
    
    



    
    
    
sensorValue = analogRead(sensorPin); //Get a value between 0 and 1023 from the analog pin connected to the anemometer
sensorVoltage = sensorValue * voltageConversionConstant; //Convert sensor value to actual voltage

//Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
if (sensorVoltage <= voltageMin){
 windSpeed = 0; //Check if voltage is below minimum value. If so, set wind speed to zero.
}
else{
  windSpeed = (sensorVoltage - voltageMin)*windSpeedMax/(voltageMax - voltageMin); //For voltages above minimum value, use the linear relationship to calculate wind speed.
  windSpeed2 = windSpeed*3.6;
}

if ((RainHigh==false)&&(digitalRead(RainPin)==HIGH)){
   RainHigh=true;
   RainAccum+=LowAmt;
   if (RainAccum==10){ 
     DiezMl+=1;
     RainAccum=0;
     }
}
if ((RainHigh==true)&&(digitalRead(RainPin)==LOW)){
   RainHigh=false;
   RainAccum+=HiAmt;
   if (RainAccum==10){ 
     DiezMl+=1;
     RainAccum=0;
     }
}



    
    
    
   /*
    // get the value we want to append to our spreadsheet
    unsigned long sensorValue = getSensorValue(); --------------------------------------------------------------------------------------------
    unsigned long sensorValue1 = getSensorValue(); */
    

    Serial.println("Appending value to spreadsheet...");

    // we need a Process object to send a Choreo request to Temboo
    TembooChoreo AppendRowChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked and repopulated with
    // appropriate arguments each time its run() method is called.
    AppendRowChoreo.begin();
    
    // set Temboo account credentials
    AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);
    
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
    String rowData(now); /*****************************************************************************************************/
    rowData += ",";
    rowData += windSpeed;
    rowData += ",";
    rowData += RainAccum;

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


