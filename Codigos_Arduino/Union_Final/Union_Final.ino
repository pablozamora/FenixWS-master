#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <FileIO.h>
#include <Bridge.h>
Adafruit_ADS1115 ads1115;

#define RainPin 5  //pin para conectar sensor de precipitaci+on

//Rain variable
bool RainHigh=false;
const float LowAmt=1.0; //when rain is low, takes this ml to trip
const float HiAmt=1.0;    //when rain is high, takes this ml to trip
float RainAccum=0.0;     //Rain accumulator since start of sample
float DiezMl=0.0;       //10 mL accumulator

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
  
  // Paquete de datos de sensores
  String dataString;
  dataString += RadiationSolar;
  dataString += ", ";
  dataString += WindSpeed;
  dataString += ", ";
  dataString += DiezMl;
  dataString += ", ";
  dataString += RainAccum;
  //dataString += ", ";
//  dataString += SHT71;
  

 // Almacenamiento de datos documento excel 
  delay(1000);
  File dataFile = FileSystem.open("/mnt/sda1/arduino/www/prueba1.csv", FILE_APPEND);
  if (dataFile){
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  }
  else{
    Serial.println("FAIL");
  }
  
  //Despliegue de Datos monitor serial
  Serial.print("Radiacion Solar (W/m2) : "); 
  Serial.println(RadiationSolar);
  Serial.print("Velocidad del viento (m/s): ");
  Serial.println(WindSpeed);
  Serial.print("Acumulador: ");
  Serial.print(RainAccum);
  Serial.print("\t");
  Serial.print("10 mL Acumulador: ");
  Serial.println(DiezMl);
  Serial.println("//////////////////////////// ");
  Serial.println(" ");
  
  delay(2000);
}
