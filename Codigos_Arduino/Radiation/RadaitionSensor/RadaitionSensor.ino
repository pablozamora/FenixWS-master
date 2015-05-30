
int SPoutput = 0;    // select the input pin for the potentiometer
int RadiationValue = 0;  // variable to store the value coming from the sensor
float CalibrationFactor = 0.5;
float voltage;

void setup() {
  // declare the ledPin as an OUTPUT:
  Serial.begin(9600);       // use the serial port
}

void loop() {
  // read the value from the sensor:
  SPoutput = analogRead(A0);
  voltage = (SPoutput/1023.0)*5;
  RadiationValue = voltage*1000*CalibrationFactor;
  Serial.println(RadiationValue);
  delay(2000);
}
