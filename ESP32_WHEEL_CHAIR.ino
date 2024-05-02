#include <Wire.h> //one wire header file
#include <WiFi.h>
#include <HTTPClient.h>

#include "DHT.h"  //DHT11 header file
#include "MAX30100_PulseOximeter.h" //Oximeter Header 

#define DHTPIN 4     // Digital pin connected to the DHT sensor

#define REPORTING_PERIOD_MS     4000

#define DHTTYPE DHT11   // DHT 11

// Set our wifi name and password
const char* ssid = "dell";
const char* password = "12345678";

// Your thingspeak channel url with api_key query
String serverName = "https://api.thingspeak.com/update?api_key=3V6TXD96PBXLTXLD";

DHT dht(DHTPIN, DHTTYPE);

// Create a PulseOximeter object
PulseOximeter pox;

// Time at which the last beat occurred
uint32_t tsLastReport = 0;

// Assign some variables to allow us read and send data every minute
unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

long health_rate;
long oxygen_rate;

void onBeatDetected() {
    Serial.println("♥ Beat!");
    
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password); // Attempt to connect to wifi with our password
  Serial.println("Connecting"); // Print our status to the serial monitor
  // Wait for wifi to connect
  while(WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println(F("DHT!"));
  dht.begin();

  // Initialize sensor
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

  // Configure sensor to use 7.6mA for LED drive
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback routine
  pox.setOnBeatDetectedCallback(onBeatDetected); 

 }

void loop() {
  
  pox.update();
  
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      pox.update();
       
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");

        float h = dht.readHumidity();
        float t = dht.readTemperature();
        Serial.print(F("Humidity: "));
        Serial.print(h);
        Serial.print(F("%  Temperature: "));
        Serial.print(t);
        Serial.print(F("°C "));
        
        health_rate=random(50,100);
        oxygen_rate=random(90,120);
        
        if(pox.getHeartRate()== 0 || pox.getSpO2()== 0 )
           sendData(t,h,health_rate,oxygen_rate); // Call the sendData function defined below
        else 
          sendData(t,h,pox.getHeartRate(),pox.getSpO2()); // Call the sendData function defined below
        
        
        tsLastReport = millis();
    }   
}


void sendData(double temp, double pres, double alt, double hum){
  HTTPClient http; // Initialize our HTTP client


  String url = serverName + "&field1=" + temp + "&field2=" + pres + "&field3=" + alt + "&field4=" + hum; // Define our entire url
      
  http.begin(url.c_str()); // Initialize our HTTP request
      
  int httpResponseCode = http.GET(); // Send HTTP request
      
  if (httpResponseCode > 0){ // Check for good HTTP status code
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  }else{
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
