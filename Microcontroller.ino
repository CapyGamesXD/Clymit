#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include  <Adafruit_BMP280.h>
#include <ESPmDNS.h>

Adafruit_BMP280 bmp; // I2C Interface



//Default config stuff XD

                  
//CHANGE THE PASSWORD TO FIT YOUR INSTANCE! 
String ssid = "WIFI_SSID_HERE";
String password = "WIFI_PASSWORD_HERE";





//Initialise a web server 
WebServer server(80);




void handleWeather () {

double temperature = bmp.readTemperature();
double pressure = bmp.readPressure()/100;
double altitude = bmp.readAltitude(1023); 
 


 //Declare a new JSON document of 128 bytes (more than enough for basic weather stuff :D)
  JsonDocument weatherDoc;

  //Assign the document some classes! Add more if you want more info, super simple! :)
  weatherDoc["temp"] = temperature;
  weatherDoc["pressure"] = pressure;
    weatherDoc["altitude"] = altitude;
  //Convert the data to JSON
  String jsonString;
  serializeJson(weatherDoc, jsonString);

  //Post the header and the data
  server.sendHeader("Content-Type", "application/json");
  server.send(200, "application/json", jsonString);

  
}


void setup() {

  //Start the console and delay 0.5 seconds to allow it to start properly.
  Serial.begin(9600); 
  delay(500); 


  delay(2000);

if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor,  check wiring!"));
    while (1);
  }


  //Credit to 'https://projecthub.arduino.cc/SurtrTech/bmp280-measure-temperature-pressure-and-altitude-6002cd' for this default config code alongside other snippets!
bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500);  /* Standby time. */

  //Connect it to WiFi
  WiFi.begin(ssid, password);


  //Check the Wi-Fi status, report connecting until it's connected.
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Awaiting wifi connection... ");
  }
  MDNS.begin("clymit");

  //Assign the server to the 192.168.X.XXX/weather page
  server.on("/", HTTP_GET, handleWeather);

  //Begin
  server.begin();

  //Output the IP.
  Serial.println(WiFi.localIP());

 
}

void loop() {

  //If a client connects, handle it with server's built-in functions
server.handleClient();
}
