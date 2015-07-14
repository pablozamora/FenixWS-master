#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <FileIO.h>
#include <Bridge.h>
#include "RTClib.h"
Adafruit_ADS1115 ads1115;

#define RainPin 5
RTC_DS1307 RTC;
int dataPin = 9;
int sckPin = 8;

//SHT71 Sensor Coefficients
const float C1=-2.0468; // for 12 Bit
const float C2=0.0367; // for 12 Bit
const float C3=-0.0000015955; // for 12 Bit
const float D1=-40.1; // for 14 Bit @ 5V
const float D2=0.01; // for 14 Bit DEGC
const float T1=0.01; // for 14 Bit @ 5V
const float T2=0.00008; // for 14 Bit @ 5V

//Rain variable
bool RainHigh=false;
const float LowAmt=1.0; //when rain is low, takes this ml to trip
const float HiAmt=1.0;    //when rain is high, takes this ml to trip
float RainAccum=0.0;     //Rain accumulator since start of sample

//ADC para SP-215
int adc0 = 0;
float CalibrationFactor = 0.5;  //Factor de calibración de hoja de datos

//ADC para WindSpeedSensor
int adc1 = 0;

void setup(void){  
  Bridge.begin();
  Serial.begin(9600);
  FileSystem.begin();
  ads1115.begin();
  
  //RTC
  Wire.begin(); // Inicia el puerto I2C
  RTC.begin(); // Inicia la comunicación con el RTC
  
  //SHT71
  pinMode(dataPin,OUTPUT);
  pinMode(sckPin,OUTPUT);
  resetSHT();
  
  //SD 
  while(!Serial);  // wait for Serial port to connect.
  Serial.println("Filesystem datalogger\n");
  
  // Rain get start state
  if (digitalRead(RainPin)==HIGH){
    RainHigh=true;
  }
  else{
    RainHigh=false;
  }
}
 
void loop(void){
  // Constantes datalogger
  int16_t adc0, adc1; //16bit integer
  float SPvoltage; //variable guarda nivel de tensión del SP-215
  float Windvoltage; //variable guarda nivel de tensión de WindSpeedSensor
  float RadiationSolar; //variable que almacena valor de radiación solar
  float WindSpeed; //variable que almacena valor de velocidad del viento
  
  //RTC
  DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
  String segundo;
  String minuto;
  String mes;
  String dia;
  String hora;
  segundo += now.second();
  minuto += now.minute();
  mes += now.month();
  dia += now.day();
  hora += now.hour();
  
  
  //Calculo SHT71
  delay(4000);
  resetSHT();
  int temp_raw = getTempSHT(); // get raw temperature value
  float temp_degc = (temp_raw * D2) + D1; // Unit Conversion - See datasheet
  resetSHT();
  int rh_raw = getHumidSHT(); // get raw Humidity value
  float rh_lin = C3 * rh_raw * rh_raw + C2 * rh_raw + C1; // Linear conversion
  float rh_true = (((temp_degc - 25) * (T1 + T2 * rh_raw)) + rh_lin); // Temperature compensated RH
  
  //Calculo valores de tension SP-215 y velocidad del viento con ADS1115 Adafruit, A0 -> SP-215, A1 -> Windsensor
  adc0 = ads1115.readADC_SingleEnded(0);
  adc1 = ads1115.readADC_SingleEnded(1);
  SPvoltage= adc0 *(0.000188);
  Windvoltage= adc1 *(0.000188);
  
  //Calculo Radiación solar SP-215
  RadiationSolar = SPvoltage*1000*CalibrationFactor;
  
  //Calculo velocidad viento
  WindSpeed=(Windvoltage*15)-15;
  
  // Calculo de precipitación 
  if ((RainHigh==false)&&(digitalRead(RainPin)==HIGH)){
    RainHigh=true;
    RainAccum+=LowAmt;
  }
  
  if ((RainHigh==true)&&(digitalRead(RainPin)==LOW)){
    RainHigh=false;
    RainAccum+=HiAmt;
  }
  
  // Paquete de datos de sensores
  //int sensor = analogRead(A1);
  String dataString;
  dataString += now.year();
  dataString += "/";
  dataString += now.month();
  dataString += "/";
  dataString += now.day();
  dataString += ",";
  dataString += " ";
  dataString += now.hour();
  dataString += ":";
  dataString += now.minute();
  dataString += ":";
  dataString += now.second();
  dataString += ",";
  dataString += " ";
  dataString += RadiationSolar;  // Radiación solar
  dataString += ", ";
  dataString += WindSpeed;  // Velocidad viento
  dataString += ", ";
  dataString += RainAccum;  // Precipitación
  dataString += ", ";
  dataString += temp_degc; // Temperatura
  dataString += ", ";
  dataString += rh_true; // Humedad relativa
  



 // Almacenamiento de datos documento excel (1 archivo por mes)
  
  //DATOS DEL MES DE ENERO
  if ( mes == "1"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_ENERO.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
 
 //DATOS DEL MES DE FEBRERO
  if ( mes == "2"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_FEBRERO.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }

 //DATOS DEL MES DE MARZO
  if ( mes == "3"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_MARZO.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
  
  //DATOS DEL MES DE Abril
  if ( mes == "4"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_ABRIL.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
    
  //DATOS DEL MES DE MAYO
  if ( mes == "5"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_MAYO.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
    
   //DATOS DEL MES DE JUNIO
  if ( mes == "6"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_JUNIO.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
  
  //DATOS DEL MES DE JULIO
  if ( mes == "7"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_JULIO.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
  
  //DATOS DEL MES DE AGOSTO
  if ( mes == "8"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_AGOSTO.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
  
  //DATOS DEL MES DE SETIEMBRE
  if ( mes == "9"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_SETIEMBRE.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
  
  //DATOS DEL MES DE OCTUBRE
  if ( mes == "10"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_OCTUBRE.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
  
  //DATOS DEL MES DE NOVIEMBRE
  if ( mes == "11"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_NOVIEMBRE.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
  
  //DATOS DEL MES DE DICIEMBRE
  if ( mes == "12"){
    //Alamcenamiento de archivos cada 10 minutos 
    if ( ((minuto== "10") || (minuto== "20") || (minuto== "30") || (minuto== "40") || (minuto== "50"))  & ((segundo == "10") || (segundo == "11"))){
      File dataFile = FileSystem.open("/mnt/sda1/arduino/www/estacionCATIE_DICIEMBRE.xls", FILE_APPEND);
      if (dataFile){
        dataFile.println(dataString);
        dataFile.close();
        Serial.println(dataString);
      }
      else{
        Serial.println("FAIL");
      }
    }
  }
    
    
 //Envío de email automático para alertar de creación de archivo al finalizar el mes
    if ((dia=="1") & (hora=="4") & (minuto=="20") & (segundo=="10") || (segundo=="11")){
      Process p;
      p.runShellCommand("cat correo_enero.txt | ssmtp jhonnyrojas.d@gmail.com"); // ********** CREAR ARCHIVO correo_enero.txt en /root/tmp/ 
      Serial.println("DATOS del mes de enero Almacenados Correctamente");
      Serial.println(" ");
    }
   
  //Despliegue de Datos monitor serial
  Serial.print(dataString);
  Serial.print("Radiacion Solar (W/m2) : ");
  Serial.println(RadiationSolar);
  Serial.print("Velocidad del viento (m/s): ");
  Serial.println(WindSpeed);
  Serial.print("Acumulador: ");
  Serial.println(RainAccum);
  Serial.print("Temperatura: ");
  Serial.println(temp_degc);
  Serial.print("Humedad relativa: ");
  Serial.println(rh_true);
  Serial.print("\t");
  Serial.println("//////////////////////////// ");
  Serial.println(" ");
  
  delay(1000);
}

//Funciones SHT71

void resetSHT(){
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
void startSHT(){
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

void writeByteSHT(byte data){ 
  pinMode(sckPin,OUTPUT);
  pinMode(dataPin,OUTPUT); 
  digitalWrite(dataPin,LOW);
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
  while(digitalRead(dataPin) == 1 ){
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
int readByte16SHT(){
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

int getTempSHT(){
  startSHT();
  writeByteSHT(B0000011);
  return readByte16SHT();
}

int getHumidSHT(){
  startSHT();
  writeByteSHT(B0000101);
  return readByte16SHT();
}
