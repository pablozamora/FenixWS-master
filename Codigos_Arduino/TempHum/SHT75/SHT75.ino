
/*
* Lee la sñ de - sensor de temperatura
*              - sensor de humedad Sencera
*              - sensor de Tª y humedad Sensirion SHT75
* y compara valores 
*
* Presenta por pantalla los resultados
*
* Acumula 100 muestras y hace un promedio, para mayor precision
*
* Conexion sensor temp: negro a gnd, morado al pin ANALOGICO 0 y rojo a 5 volt
* Conexion sensor humd: + a 5 volt, - a gnd y V al pin ANALOGICO 1
*
* LEDs: rojo a 9 y verde al 10 
*
* Conexion del Sensirion: negro a GND, rojo a 5 volt, naranja al pin DIGITAL 3 y blanco al pin DIGITAL 4
* 
* Pongo 2 Cs: puenteando los 5 vols-GND y antes del pin de entrada
*/        

#include <math.h>

// comandos para el Sensirion
int temperatureCommand  = B00000011;  // comando para leer temperatura 
int humidityCommand     = B00000101;  // comando para leer humedad 
int comandoEstatus      = B00000111;  // comando para leer registro de estatus

// especifico para el sensor Sensirion
int clockPin = 3;  // pin para el reloj // HAY Q CAMBIAR LOS PINEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEES
int dataPin  = 4;  // pin para datos 
int ack;  // detectar posible ocurrencia de errores

// para los 2 sensores independientes
int temPin = 0;    // pin para el sensor de temperatura
int humPin = 1;    // pin para el sensor de humedad
int rojo  = 9;     // los LEDs van en las salidas PWM 9..
int verde = 10;    // ..y 10
int valT = 0;      // variable to store the value coming from the sensor
int valH = 0;      // idem
int Nlectura = 0;  // esto cuenta el numero de lecturas
int aux = 0;       // auxiliar q se resetea cada 100 lecturas
float valTFloat = 0.0;   // aqui pongo la transformacion a flotante de 'val'
float valHFloat = 0.0;   // idem
float milivoltsT = 0.0;  // transformo a milivolts
float milivoltsH = 0.0;  // idem
float temp = 0.0;       // aqui va la temperatura en C
float hum  = 0.0;       // idem en %
float acumT = 0.0;       // aqui voy acumulando hasta 100 lecturas
float acumH = 0.0;       // idem
float tempMedia = 0.0;  // el promedio de 100 lecturas
float humMedia = 0.0;   // idem
float umbralTemperatura = 27.0;  // fijo un umbral para iluminar LEDs
float umbralHumedad     = 50.0;  // fijo un umbral para iluminar LEDs
int   espera = 3;        // lo que se espera
float diferenciaT = 0.0;  // para comparar valores de temperatura
float diferenciaH = 0.0;  // para comparar valores de humedad
float RHtrue = 0.0;       // aqui se pone la humedad relativa compensada por humedad
float Rocio  = 0.0;       // y aqui la temperatura de rocio
int numerobitsT = 14;     // especifico el numero de bits con los q voy a medir (es la precision), por defecto 14
int numerobitsH = 12;     // especifico el numero de bits con los q voy a medir (es la precision), por defecto 12


void setup() {
 pinMode(verde,  OUTPUT);   // sets the pin as output
 pinMode(rojo,   OUTPUT);   // sets the pin as output
 Serial.begin(9600); // open serial at 9600 bps
} 

void loop() {
 // 1 - esto del principio es para el Sensirion
 delay(12);   // hay q esperar 12 ms desde el start up
 
 
 // leo el registro de estatus, por defecto todo 0s (opcional, para verificar)
 leerEstatus();
 
 // leer la temperatura (por defecto en 14 bits) y convertirla a grados Celsius
 valT = leerValorTemperatura();                   // lee el dato (integer)
 temp = calcularTemperatura(valT, numerobitsT);   // lo traduzco a grados Celsius
 diferenciaT = temp;                              // pongo de momento un dato
 
 // leer la humedad (por defecto en 12 bits) y convertirla a HR en tanto por ciento
 valH = leerValorHumedad();                       // leo el dato (integer)
 hum = calcularHR(valH, numerobitsH);             // lo traduzco a HR en tanto por ciento
 diferenciaH = hum;                               // pongo de momento un dato
 
 // calculos adicionales
 RHtrue = calcularHumedadVerdadera(temp, valH, hum, numerobitsH);
 Rocio  = calcularRocio(hum, temp);
 
 // saco por pantalla
 representar(temp, hum, RHtrue, Rocio, valT, valH);
 
 // 2 - a partir de aqui, dentro del loop, es para los sensores independientes
 Nlectura = Nlectura + 1;    // incrementos
 valT = analogRead(temPin);  // leemos el calor de los sensores analogicos
 // delay(500);      // es opcional
 valH = analogRead(humPin);
 valTFloat = (float) valT;
 valHFloat = (float) valH; 
 milivoltsT = valTFloat * 5 * 1000 / 1023;  // calculo los volts q llegan
 milivoltsH = valHFloat * 5 * 1000 / 1023;
 temp = milivoltsT / 10.0;    // convierto a temperatura
 hum = ((milivoltsH/1000.0) - 0.78) * 31.84;
 diferenciaT -= temp;
 diferenciaH -= hum;  
 
 // saco por pantalla los datos decimales calculados
 Serial.print("Independientes (T, HR) :  ");
 Serial.print(temp);  
 Serial.print(" C   ");
 Serial.print(hum);
 Serial.print(" %             diferencia de temp = ");
 Serial.print(diferenciaT);
 Serial.print(" y de hum = ");
 Serial.println(diferenciaH);

/*
 // hago una media de los ultimos 100 valores (opcional)
 aux = aux + 1; 
 acumT = acumT + temp;        // lo voy acumulando
 acumH = acumH + hum;
 if (aux == 100) {
   tempMedia = acumT / 100.0;
   humMedia  = acumH / 100.0;
   Serial.println("******************************************************");
   Serial.print("Temperatura promedio en las ultimas 100 muestras  =  ");
   Serial.println(tempMedia);
   Serial.print("Humedad promedio en las ultimas 100 muestras  =  ");
   Serial.println(humMedia);
   Serial.println("******************************************************");
   acumT = 0.0;
   acumH = 0.0;
   aux = 0;
   delay(espera);
 }
*/  
 // para iluminar los LEDs segun el umbral de temperatura o de humedad
 if (temp < umbralTemperatura) {   // si esto es q no se supera el umbral y 
   analogWrite(verde,1000);        // escribo en uno cero y en otro el valor
   analogWrite(rojo,0);}
 else {
   analogWrite(rojo,1000);
   analogWrite(verde,0);}    

 // esperamos X segundos hasta la proxima medida (minimo 1000 milisec para no calentar el Sensirion)
 delay(1500);
} 


// funciones para lectura / escritura del Sensirion

// leer el valor de temperatura que da el sensor (hace todo el proceso transparente)
int leerValorTemperatura() {
 int valT = 0;
 
 sendCommandSHT(temperatureCommand, dataPin, clockPin); // envio peticion
 waitForResultSHT(dataPin);                             // espero a q tenga el resultado
 valT = getData16SHT(dataPin, clockPin);                // capturo el valor
 skipCrcSHT(dataPin, clockPin);                         // me salto la CRC
 return valT;
}

// leer el valor de humedad que da el sensor (hace todo el proceso transparente)
int leerValorHumedad() {
 int valH = 0;
 
 sendCommandSHT(humidityCommand, dataPin, clockPin);    // envio peticion
 waitForResultSHT(dataPin);                             // espero a q tenga el resultado
 valH = getData16SHT(dataPin, clockPin);                // capturo el valor
 skipCrcSHT(dataPin, clockPin);                         // me salto la CRC
 return valH;
}

// enviar comando al Sensirion 
void sendCommandSHT(int command, int dataPin, int clockPin) { 
 int ack;

 // empieza la transmision
 pinMode(dataPin,  OUTPUT);
 pinMode(clockPin, OUTPUT);
 digitalWrite(dataPin,  HIGH);
 digitalWrite(clockPin, HIGH);
 delayMicroseconds(1);
 digitalWrite(dataPin,  LOW);
 digitalWrite(clockPin, LOW);
 delayMicroseconds(1);
 digitalWrite(clockPin, HIGH);
 digitalWrite(dataPin,  HIGH);
 delayMicroseconds(1);
 digitalWrite(clockPin, LOW);
 
 // escribir el comando (los 3 primeros bits deben ser la direccion (siempre 000), y los ultimos 5 bits son el comando)
 shiftOut(dataPin, clockPin, MSBFIRST, command);
 
 // verificar q obtenemos los ACKs adecuados
 digitalWrite(clockPin, HIGH);
 pinMode(dataPin, INPUT);
 ack = digitalRead(dataPin);
 if (ack != LOW)
   Serial.println("ACK error 0");
 digitalWrite(clockPin, LOW);
 ack = digitalRead(dataPin);
 if (ack != HIGH)
   Serial.println("ACK error 1");
}


// esperar a la respuesta del Sensirion
void waitForResultSHT(int dataPin) {
 int ack;

 pinMode(dataPin, INPUT);
 ack = digitalRead(dataPin);
 while (ack == HIGH){
   ack = digitalRead(dataPin);
   }
}

// leemos dato del data stream del Sensirion
int shiftIn(int dataPin, int clockPin, int numBits) {
 int ret = 0;

 for (int i=0; i<numBits; i++) {
   digitalWrite(clockPin, HIGH);
   delayMicroseconds(1000);
   ret = ret * 2 + digitalRead(dataPin);
   digitalWrite(clockPin, LOW);
 }
 return ret;
}

// obtener dato del Sensirion
int getData16SHT(int dataPin, int clockPin) {
 int val, aux;

 // obtener los MSB (bits mas significativos)
 pinMode(dataPin, INPUT);
 pinMode(clockPin, OUTPUT);
 val = shiftIn(dataPin, clockPin, 8);
 val *= 256; // esto es equivalente a desplazar a la izq, SHIFT: val << 8;
 
 // escucho el 1 de propina tras el MSB
 aux = digitalRead(dataPin);
 while (aux != 1) {aux = digitalRead(dataPin);}
 
 // enviar el ACK, MUCHO CUIDADO
 pinMode(dataPin, OUTPUT);
 digitalWrite(clockPin, HIGH);
 digitalWrite(dataPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(dataPin, LOW);
 digitalWrite(clockPin, LOW);
 
 // obtener los LSB (bits menos significativos)
 pinMode(dataPin, INPUT);
 pinMode(clockPin, OUTPUT);
 val |= shiftIn(dataPin, clockPin, 8);
 return val;
}

// obtener el estatus del Sensirion
int getDataEstatus(int dataPin, int clockPin) {
 int val, aux;

 // obtener los 8 bits
 pinMode(dataPin, INPUT);
 pinMode(clockPin, OUTPUT);
 val = shiftIn(dataPin, clockPin, 8);
 
 // escucho el 1 de propina tras el less significant bit
 aux = digitalRead(dataPin);
 while (aux != 1) {aux = digitalRead(dataPin);}
 
 // enviar el ACK, MUCHO CUIDADO
 pinMode(dataPin, OUTPUT);
 digitalWrite(clockPin, HIGH);
 digitalWrite(dataPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(dataPin, LOW);
 digitalWrite(clockPin, LOW);
}

// saltarse (no solicitar) la comprobacion CRC
void skipCrcSHT(int dataPin, int clockPin) { 
 pinMode(dataPin, OUTPUT); 
 pinMode(clockPin, OUTPUT); 
 digitalWrite(dataPin, HIGH); 
 digitalWrite(clockPin, HIGH); 
 digitalWrite(clockPin, LOW); 
}

// calcular la temperatura en grados Celsius
float calcularTemperatura(int valT, int numerobitsT) {
 float d1, d2 = 0.0;
 float temperatura = 0.0;
 
 d1 = -40.1;      // es para el caso de alimentacion = 5 volts
 // d1 = -39.7;   // alimentacion = 3.5 volts
 // d1 = -39.6;   // alimentacion = 3.0 volts
 
 if (numerobitsT == 14) {
   d2 = 0.01;
 } else {
   d2 = 0.04;
 }
 temperatura = d1 + d2 * valT;
 return temperatura;
}

// calcular la humedad relativa en tanto por ciento
float calcularHR(int valH, int numerobitsH) {
 float c1 = -4.0;
 float c2, c3 = 0.0;
 float HR = 0.0;
 
 if (numerobitsH == 12) {
   c2 = 0.0405;
   c3 = -0.0000028;
 } else {
   c2 = 0.6480;
   c3 = -0.00072;
 }
 HR = c1 + c2 * valH + c3 * valH * valH;
 return HR;
}

// calcular el punto de rocio (T_dew)
float calcularRocio(float RH, float T) {
 float Tn1 = 243.12;  // de 0 a 50 grados C
 float Tn2 = 272.62;  // de -40 a 0 grados C
 float m1 = 17.62;    // de 0 a 50 grados C
 float m2 = 22.46;    // de -40 a 0 grados C
 
 float Ro = 0.0;  // variables
 float Tn = 0.0;
 float m  = 0.0;
 
 if (T < 0.0) {   // elijo segun por encima de 0 o no
   Tn = Tn1;
   m  = m1;
 } else {
   Tn = Tn2;
   m  = m2;
 }
 Ro = Tn * (log(RH/100) + ((m*T) / (Tn+T))) / (m - log(RH/100) - ((m*T) / (Tn*T)));
 return Ro;  // devuelvo
}

// sacar por pantalla (se puede comentar al gusto, para no sacar tanta info)
void representar(float T, float HR, float RHtrue, float Rocio, int valT, int valH) {
 Serial.println("*************************************************************************");
 // escribo los datos binarios q he leido (opcional)
 Serial.print("val temp = ");
 Serial.print(valT);
 Serial.print(" = ");
 Serial.print(valT, BIN);
 Serial.print(", val hum = ");
 Serial.print(valH);
 Serial.print(" = ");
 Serial.print(valH, BIN);
 // datos adicionales (opcional)
 Serial.print("    HR verdadera = ");
 Serial.print(RHtrue);
 Serial.print(" %,  T de rocio = ");
 Serial.print(Rocio);
 Serial.println(" C");
 // datos importantes
 Serial.print("Sensirion (T, HR) :       ");
 Serial.print(temp);
 Serial.print(" C   "); 
 Serial.print(hum);
 Serial.println(" %");  
}

// calcular la humedad relativa verdadera corregido por la temperatura
float calcularHumedadVerdadera(float T, int valH, float HR, int numerobitsH) {
 float t1 = 0.01;
 float t2 = 0.0;
 float HRtrue = 0.0;
 
 if (numerobitsH == 12) {  // el numero de bits para medir la humedad son 8 o 12
   t2 = 0.00008;
 } else {
   t2 = 0.00128;
 }
 
 HRtrue = (T - 25.0) * (t1 + t2 * valH) + HR;
 return HRtrue;
}

// leer la palabra de estatus
void leerEstatus() {
 int val= 0;

 sendCommandSHT(comandoEstatus, dataPin, clockPin);
 waitForResultSHT(dataPin); 
 val = getDataEstatus(dataPin, clockPin);
 skipCrcSHT(dataPin, clockPin);
 Serial.print("estatus = ");
 Serial.println(val, BIN);
}
