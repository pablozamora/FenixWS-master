/*

Anemometer Tutorial

This code takes the output from an Adafruit anemometer (product ID 1733) and converts it into a wind speed.

The circuit:

Created 11 November 2014
By Joe Burg
Modified 11 November 2014
By Joe Burg

Hardware instructions and background information can be found at: http://www.hackerscapes.com/2014/11/anemometer/ ‎

Special thanks to Adafruit forum users shirad and adafruit_support_mike for help in trouble shooting code and hardward, including provided code of their own for comparison and inspiration.
*/

//Setup Variables

const int sensorPin = A0; //Defines the pin that the anemometer output is connected to
int sensorValue = 0; //Variable stores the value direct from the analog pin
float sensorVoltage = 0; //Variable that stores the voltage (in Volts) from the anemometer being sent to the analog pin
float windSpeed = 0; // Wind speed in meters per second (m/s)

float voltageConversionConstant = .004882814; //This constant maps the value provided from the analog read function, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V
int sensorDelay = 1000; //Delay between sensor readings, measured in milliseconds (ms)

//Anemometer Technical Variables
//The following variables correspond to the anemometer sold by Adafruit, but could be modified to fit other anemometers.

float voltageMin = 1.02; // Mininum output voltage from anemometer in mV.
float windSpeedMin = 0; // Wind speed in meters/sec corresponding to minimum voltage

float voltageMax = 5; // Maximum output voltage from anemometer in mV.
float windSpeedMax = 30; // Wind speed in meters/sec corresponding to maximum voltage



void setup() 
{              
  Serial.begin(9600);  //Start the serial connection
}


void loop() 
{
sensorValue = analogRead(sensorPin); //Get a value between 0 and 1023 from the analog pin connected to the anemometer

sensorVoltage = sensorValue * voltageConversionConstant; //Convert sensor value to actual voltage

//Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
if (sensorVoltage <= voltageMin){
 windSpeed = 0; //Check if voltage is below minimum value. If so, set wind speed to zero.
}else {
  windSpeed = (sensorVoltage - voltageMin)*windSpeedMax/(voltageMax - voltageMin); //For voltages above minimum value, use the linear relationship to calculate wind speed.
}
 //Print voltage and windspeed to serial
  Serial.print("Voltage en bits: ");
  Serial.print(sensorValue);
  Serial.print("\t");
  Serial.print("Voltage: ");
  Serial.print(sensorVoltage);
  Serial.print("\t"); 
  Serial.print("Wind speed: ");
  Serial.print(windSpeed);
  Serial.println(" m/s."); 
 
 delay(sensorDelay);
}
