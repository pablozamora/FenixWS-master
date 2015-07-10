#include <Bridge.h>
#include <FileIO.h>
#include <Console.h>
#include <Wire.h> 
#include "RTClib.h"
#include <Process.h>

RTC_DS1307 RTC;

void setup(){
   Bridge.begin();
   Wire.begin(); // Inicia el puerto I2C
   RTC.begin(); // Inicia la comunicación con el RTC
   Serial.begin(9600);
   FileSystem.begin();
   
   while (!Serial);
   Serial.println("ANALOG READ with datalogger\n");
}
void loop(){
  //int sensor = analogRead(A1);
  DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
  String dataString;
  String hora;
  hora += now.minute();
  hora += now.second();
  dataString += now.year(); // Año
  dataString += "/";
  dataString += now.month(); // Mes
  dataString += "/";
  dataString += now.day(); // Dia
  dataString += " ";
  dataString += now.hour(); // Horas
  dataString += ":";
  dataString += now.minute(); // Minutos
  dataString += ":";
  dataString += now.second(); // Segundos
  dataString += " ";
  dataString += hora;
  dataString += " ";
  
  Process p;
  if (hora == "2910"){
    p.runShellCommand(echo -e "to: jhonnyrojas.d@gmail.com \n subject: PRUEBA \n"| (cat - && uuencode /path/to/attachment attachment.test.txt) | ssmtp jhonnyrojas.d@gmail.com);
    Serial.println("Correo Enviado");
    Serial.println(" ");
  }
  
  for (int analogPin = 0; analogPin < 3; analogPin++){
    int sensor=analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 2){
      dataString += ",";
    }
  }
  //dataString += String(sensor);
  File dataFile = FileSystem.open("/mnt/sda1/arduino/www/prueba_miercoles.xls", FILE_APPEND);
  if (dataFile){
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  }
  else{
    Serial.println("FAIL");
  }
  delay(1000);
}

