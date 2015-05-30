#define RainPin 5 

//Radiation Variables
int SPoutput = 0;    // select the input pin for the potentiometer
int RadiationValue = 0;  // variable to store the value coming from the sensor
float CalibrationFactor = 0.5; //factor de calibracion de sensor sp-215
float voltage; 

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

void setup() 
{              
  Serial.begin(9600);  //Start the serial connection
  
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

//********************************************************* WIND SPEED *********************************************************************************

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


//********************************************************* RAIN GAUGE *********************************************************************************

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

//********************************************************* RADIATION *******************************************************************************
SPoutput = analogRead(A0);
voltage = (SPoutput/1023.0)*5;
RadiationValue = voltage*1000*CalibrationFactor;


//***************************************************************************************************************************************************

 //Print voltage and windspeed to serial 
  Serial.print("Velocidad del viento: ");
  Serial.print(windSpeed);
  Serial.print(" m/s. or ");
  Serial.print(windSpeed2);
  Serial.println(" km/h.");
  Serial.print("Acumulador: ");
  Serial.print(RainAccum);
  Serial.print("\t");
  Serial.print("10 mL Acumulador: ");
  Serial.println(DiezMl);
  Serial.print("Radiación Solar ");
  Serial.print(RadiationValue);
  Serial.print("W/m2");
 
 delay(sensorDelay);
}

