#include <Bridge.h>
#include <FileIO.h>
#include <Console.h>


void setup(){
   Bridge.begin();
   Serial.begin(9600);
   FileSystem.begin();
   
   while (!Serial);
   Serial.println("ANALOG READ with datalogger\n");
}
void loop(){
  //int sensor = analogRead(A1);
  String dataString;
  dataString += getTimeStamp();
  dataString += ", ";
  
  for (int analogPin = 0; analogPin < 3; analogPin++){
    int sensor=analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 2){
      dataString += ",";
    }
  }
  
  //dataString += String(sensor);
  
  delay(1000);
  
  File dataFile = FileSystem.open("/mnt/sda1/arduino/www/prueba_martes.txt", FILE_APPEND);
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

// This function return a string with the time stamp
String getTimeStamp() {
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin("date");
  time.addParameter("+%D, %T");  // parameters: D for the complete date mm/dd/yy
  //             T for the time hh:mm:ss
  time.run();  // run the command
  // read the output of the command
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n')
      result += c;
  }
  return result;
}

