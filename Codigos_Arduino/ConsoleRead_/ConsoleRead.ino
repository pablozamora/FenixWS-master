/*
  Console Read example

 Read data coming from bridge using the Console.read() function
 and store it in a string.

 To see the Console, pick your Yún's name and IP address in the Port menu
 then open the Port Monitor. You can also see it by opening a terminal window
 and typing:
 ssh root@ yourYunsName.local 'telnet localhost 6571'
 then pressing enter. When prompted for the password, enter it.

 created 13 Jun 2013
 by Angelo Scialabba
 modified 16 June 2013
 by Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/ConsoleRead

 */

#include <Console.h>

String name;
int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
unsigned sensorValue;  // variable to store the value coming from the sensor
float voltage;
float vcc;

void setup() {
  // Initialize Console and wait for port to open:
  Bridge.begin();
  Console.begin();

  // Wait for Console port to connect
  while (!Console);

  Console.println("solictar dato");
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 5110000L / result; // Back-calculate AVcc in mV
  return result;
}

void loop() {
  if (Console.available() > 0) {
    char c = Console.read(); // read the next char received
    // look for the newline character, this is the last character in the string
    if (c == '\n') {
      //print text with the name received
      // read the value from the sensor:
       vcc = readVcc()/1000.0;
       sensorValue = analogRead(sensorPin);
       voltage = (sensorValue/1023.0)*vcc;
       
  // turn the ledPin on
       digitalWrite(ledPin, HIGH);
  // stop the program for <sensorValue> milliseconds:
       delay(sensorValue);
  // turn the ledPin off:
       digitalWrite(ledPin, LOW);
  // stop the program for for <sensorValue> milliseconds:
      delay(sensorValue);
      Console.print(voltage);

    }
    else {  	 // if the buffer is empty Cosole.read() returns -1
      name += c; // append the read char from Console to the name string
    }
  } else {
    Serial.print(readVcc(), DEC);
    delay(100);
  }
}


