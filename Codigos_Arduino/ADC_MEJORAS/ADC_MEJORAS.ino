
#include <Console.h>
const uint8_t ADC_PS_16  = (1 << ADPS2);
const uint8_t ADC_PS_32  = (1 << ADPS2) | (1 << ADPS0);
const uint8_t ADC_PS_64  = (1 << ADPS2) | (1 << ADPS1);
const uint8_t ADC_PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
const uint8_t ADC_PS_BITS = ADC_PS_128;
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
  ADCSRA &= ~ADC_PS_BITS;
  ADCSRA |= ADC_PS_128;    

  // Wait for Console port to connect
  while (!Console);

  Console.println("solictar dato");
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

long readVcc() {
  long result;
  // lee la referencia de Vref
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // espera el valor de Vref
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 5110000L / result; // Vcc a mV
  return result;
}

void loop() {
  if (Console.available() > 0) {
    char c = Console.read(); // recibe dato desde la computadora
    if (c == '\n') {
      // leer valor del sensor
       vcc = readVcc()/1000.0;
       uint16_t sensorValue = analogRead(sensorPin);
       voltage = (sensorValue/1023.0)*vcc;
       
  // indicaador LED de dato
       digitalWrite(ledPin, HIGH);
       delay(sensorValue);
       digitalWrite(ledPin, LOW);
       
  // Desplegar el valor del voltaje:
      delay(sensorValue);
      Console.print(voltage);

    }
    else {  	 // si no se lee dato analogico se retorna -1
      name += c; // append the read char from Console to the name string
    }
  } else {
    Serial.print(readVcc(), DEC);
    delay(500);
  }
}

