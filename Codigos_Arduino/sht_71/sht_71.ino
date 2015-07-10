/* The MIT License (MIT)
 *
 * Copyright (c) 2014 prock
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

int dataPin = 9;
int sckPin = 8;

// SHT15 Sensor Coefficients
const float C1=-2.0468; // for 12 Bit
const float C2=0.0367; // for 12 Bit
const float C3=-0.0000015955; // for 12 Bit
const float D1=-40.1; // for 14 Bit @ 5V
const float D2=0.01; // for 14 Bit DEGC
const float T1=0.01; // for 14 Bit @ 5V
const float T2=0.00008; // for 14 Bit @ 5V


void resetSHT()
{
  pinMode(dataPin,OUTPUT);
  pinMode(sckPin,OUTPUT);
  
  shiftOut(dataPin, sckPin, LSBFIRST, 255);
  shiftOut(dataPin, sckPin, LSBFIRST, 255);
  
  digitalWrite(dataPin,HIGH);
  for(int i = 0; i < 15; i++){
     digitalWrite(sckPin, LOW);
     digitalWrite(sckPin, HIGH);
  }
}

//Specific SHT start command
void startSHT()
{
  pinMode(sckPin,OUTPUT);
  pinMode(dataPin,OUTPUT);
  digitalWrite(dataPin,HIGH);
  digitalWrite(sckPin,HIGH);
  digitalWrite(dataPin,LOW);
  digitalWrite(sckPin,LOW);
  digitalWrite(sckPin,HIGH);
  digitalWrite(dataPin,HIGH);
  digitalWrite(sckPin,LOW);
}

void writeByteSHT(byte data)
{ 
  pinMode(sckPin,OUTPUT);
  pinMode(dataPin,OUTPUT);  
  
//  digitalWrite(dataPin,LOW);
  shiftOut(dataPin,sckPin,MSBFIRST,data);
  
  pinMode(dataPin,INPUT);
  digitalWrite(dataPin,HIGH);

  //Wait for SHT15 to acknowledge by pulling line low
  while(digitalRead(dataPin) == 1);
  
  digitalWrite(sckPin,HIGH);
  digitalWrite(sckPin,LOW);  //Falling edge of 9th clock
  
  //wait for SHT to release line
  while(digitalRead(dataPin) == 0 );
 
  //wait for SHT to pull data line low to signal measurement completion
  //This can take up to 210ms for 14 bit measurments
  int i = 0;
  while(digitalRead(dataPin) == 1 )
  {
    i++;
    if (i == 255) break;
    
    delay(10);
  } 
  
  //debug
  i *= 10;
  //Serial.print("Response time = ");
  //Serial.println(i);
}

//Read 16 bits from the SHT sensor
int readByte16SHT()
{
  int cwt = 0;
  unsigned int bitmask = 32768;
  int temp;
  
  pinMode(dataPin,INPUT);
  digitalWrite(dataPin,HIGH);
  
  pinMode(sckPin,OUTPUT);
  
  digitalWrite(sckPin,LOW);
  
  for(int i = 0; i < 17; i++) {
    if(i != 8) {
      digitalWrite(sckPin,HIGH);
      temp = digitalRead(dataPin);
      //Serial.print(temp,BIN);
      cwt = cwt + bitmask * temp;
      digitalWrite(sckPin,LOW);
      bitmask=bitmask/2;
    }
    else {
      pinMode(dataPin,OUTPUT);
      digitalWrite(dataPin,LOW);
      digitalWrite(sckPin,HIGH);
      digitalWrite(sckPin,LOW);
      
      pinMode(dataPin,INPUT);
      digitalWrite(dataPin,HIGH);
    }
  }
  
  //leave clock high??
  digitalWrite(sckPin,HIGH);
  
//  Serial.println();
  
  return cwt;
}

int getTempSHT()
{
  startSHT();
  writeByteSHT(B0000011);
  return readByte16SHT();
}

int getHumidSHT()
{
  startSHT();
  writeByteSHT(B0000101);
  return readByte16SHT();
}


void setup() {
  pinMode(dataPin,OUTPUT);
  pinMode(sckPin,OUTPUT);

  Serial.begin(9600);        // connect to the serial port
  
  Serial.println("Inicializando Sensor");
  Serial.println(" ");
  resetSHT();
}

void loop () {
  delay(4000);
  Serial.println("Actualizando Dato");
  resetSHT();
  //int temp = getTempSHT();
  //Serial.print("Temprature:");
  //Serial.println(temp);
  int temp_raw = getTempSHT(); // get raw temperature value
  Serial.println("Lectura completa");
  Serial.println("Temperature(C): ");
  float temp_degc = (temp_raw * D2) + D1; // Unit Conversion - See datasheet
  Serial.println(temp_degc);
  resetSHT();
  //int temp = getHumidSHT();
  //Serial.print("Humidity:");
  //Serial.println(temp);
  int rh_raw = getHumidSHT(); // get raw Humidity value
  Serial.println("Humidity(%): ");
  float rh_lin = C3 * rh_raw * rh_raw + C2 * rh_raw + C1; // Linear conversion
  float rh_true = (((temp_degc - 25) * (T1 + T2 * rh_raw)) + rh_lin); // Temperature compensated RH
  Serial.println(rh_true);
  Serial.println(" ");  
}
