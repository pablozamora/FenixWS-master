#include <SPI.h>
#include <WiFi.h>

int status = WL_IDLE_STATUS;
char ssid[] = "ZAMORA";
char pass[] = "pczc1992";

IPAddress remoteIp(192,168,0,12);
int port = 37899;

String message = "";

WiFiClient client;
long lastSendt;

void setup()
{
    // start the serial for debugging
    Serial.begin(9600);
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);

    //check if the wifi shield is present
    if(WiFi.status() == WL_NO_SHIELD){
        Serial.println("WiFi shield not present! Press reset to try again.");
        while(true); //stops the program
    }

    connectWiFi();
    printWifiStatus();
    connectClient(3);
}

void loop(){

    if(client){

        if(millis() >= (lastSendt + 15000)){
            sendCharacter('*');
        }

        if(client.available()){

            char c = client.read();

            if(c != '\n'){
                message += c;
            }
            else{
                Serial.println("Received message: "+message);
                checkMessage();
                sendMessage(message);
                message = "";
            }
        }
    }
}

void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
}

void connectWiFi(){

    if( status != WL_CONNECTED){
        while(status != WL_CONNECTED){

            Serial.print("Attempting connection to network...");

            status = WiFi.begin(ssid, pass);
            delay(3000);

            if(status == WL_CONNECTED){
                Serial.println(" SUCSESS");
            }
            else{
                Serial.println(" FAILED");
                delay(3000);
                connectWiFi();
            }
        }
    }   
}

void connectClient(int retries){

    //Attempt connection to server

    if(retries <= 0){
        Serial.println("FAILED");
        Serial.println("Connection to server failed.");
        while(true);
    }

    Serial.print("Attempting conenction to server... ");

    if(client.connect(remoteIp, port)){
        Serial.println("SUCSESS");
        sendMessage("Hello server!");
    }
    else if(retries > 0){
        Serial.println("FAILED");
        connectClient(retries - 1);
    }

}

void checkMessage(){

    if(message == "on"){
        digitalWrite(9, HIGH);
    }

    if(message == "off"){
        digitalWrite(9, LOW);
    }
}

void sendMessage(String toSend){

    if(client){
        client.println(toSend+'\n');
        client.flush();
        Serial.println("Sendt message: "+toSend);
    }
    else{
        Serial.println("Could not send message; Not connected.");
    }
}

void sendCharacter(char toSend){

    if(client){
        client.println(toSend){
        lastSendt = millis();
    }else{
        Serial.println("Could not send character!");
    }
}
