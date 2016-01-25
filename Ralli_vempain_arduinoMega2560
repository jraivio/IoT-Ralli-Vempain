// IOT RalliDroid  
// Copyright Jukka Raivio 2016
// MIT License
// 
// Example code is based on Arduino Mega2560 
//
// This arduino demo setup is connected to esp8266 Wifi via serial. The http services is powered by esp-link. The esp unit is working as Wifi-Serial bridge.
// No code changes for the esp-link
// Github repository for esp-link https://github.com/jeelabs/esp-link

// Temperature & Humidity sensor
// DHT11 lib & setup
// Adafruit version
// https://github.com/adafruit/DHT-sensor-library

#include "DHT.h"
#define DHTPIN 2     // 2 digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Sensor update interval
unsigned long previousMillis = 0;      // will store last time sensor updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
const long interval = 10000;    
  
// Json parser lib 
// Copyright Benoit Blanchon 2014-2016
// MIT License
//
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson
// If you like this project, please add a star!
#include <ArduinoJson.h>

// Serial data handling, global variables
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
char inChar; // a char to handle incoming data

// Motor driver lib & Setup
// by Yohendry Hurtado
// https://github.com/yohendry/arduino_L298N
#include <L298N.h>
const int ENA = 8;
const int IN1 = 4;
const int IN2 = 5;
const int IN3 = 6;
const int IN4 = 7;
const int ENB = 3;
L298N driver(ENA,IN1,IN2,IN3,IN4,ENB); 
// motor millis() purposes (todo)
unsigned long m_previousMillis = 0;      // will store last time sensor updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
const long m_interval = 1000;    


void Sensor_DHT() {

    // DHT functions
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // e.g results from serial {"sensor":"temp_hum"}:[22.00,36.00]}
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    // Create Json for sensor print out
    StaticJsonBuffer<512> jsonOutBuffer;   // 514 B
    String rootJson = "";
    String arrayJson = "";
    JsonObject& root = jsonOutBuffer.createObject();
    root["sensor"] = "temp_hum";
    JsonArray& array = jsonOutBuffer.createArray();
    array.add(t);
    array.add(h);
    // Print to Serial
    root.printTo(rootJson);
    array.printTo(arrayJson);
    String JointJson = rootJson + ":" + arrayJson + "}";
    Serial1.println(JointJson);
    arrayJson = "";
    return;
}

  
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */

void serialEvent1() {
  while (Serial1.available()> 0) {
    // get the new byte:
    inChar = (char)Serial1.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void HandleIncommingJson () {

    // JSon serial read
    // print the string when a newline arrives:
    StaticJsonBuffer<512> jsonInBuffer;                 
    const char *JsonChar = inputString.c_str(); // 1 KB
    JsonObject& root = jsonInBuffer.parseObject(JsonChar);
    // Verify Json 
    if (JsonChar!=NULL && !root.success()) {
      Serial.println("parseObject() failed: ");
      Serial.println(JsonChar);
      // Clean buffer
      JsonChar = NULL;
      return;
    } 
    // Command to digital pins
    // E.g Digital pin 13 High
    // {"command":"digital","data":[13,1]}
    // ... and low
    // {"command":"digital","data":[13,0]}
    // read from json

    const char* command = root["command"];
    int pin = root["data"][0];
    int value = root["data"][1];
    int mspeed = root["mdata"][0];
    int mdelay = root["mdata"][1];

    // Control LED digital output pins 
    if (command="digital") {
          // e.g pin 12 on {"command":"digital","data":[12,1]}
          // e.g curl via esp-link >curl -i -X POST http://192.168.0.171/console/send?text=%7B%22command%22%3A%22digital%22%2C%22data%22%3A%5B12%2C1%5D%7D
          // decoder support http://meyerweb.com/eric/tools/dencoder/
          if (value == 1){
            digitalWrite(pin, HIGH);
          }
          // e.g pin 12 off {"command":"digital","data":[12,0]}
          // e.g curl via esp-link >curl -i -X POST http://192.168.0.171/console/send?text=%7B%22command%22%3A%22digital%22%2C%22data%22%3A%5B12%2C0%5D%7D
          // decoder support http://meyerweb.com/eric/tools/dencoder/
          else if (value == 0){
            digitalWrite(pin, LOW);
          }
    }
    // e.g json format {"command":"drive_fw","mdata":[150,500]}
    // e.g curl via esp-link >curl -i -X POST http://192.168.0.171/console/send?text=%7B%22command%22%3A%22drive_fw%22%2C%22mdata%22%3A%5B150%2C500%5D%7D
    if (command="drive_fw") { driver.forward(mspeed,mdelay);
    }
    // e.g json format {"command":"drive_rw","mdata":[150,500]}
    if (command="drive_rw") { driver.backward(mspeed,mdelay);
    }
    // e.g json format {"command":"drive_right","mdata":[50,500]}
    if (command="drive_right") { driver.turn_right(mspeed,mdelay);
    }
    // e.g json format {"command":"drive_left","mdata":[50,500]}
    if (command="drive_left") { driver.turn_left(mspeed,mdelay);
    }
    // e.g json format {"command":"drive_stop","mdata":[500]}
    if (command="drive_stop") { driver.full_stop(mdelay);
    }
    
     // clean buffer
     inputString="";
     stringComplete = false;
     inChar = NULL;
     JsonChar = NULL;
     return;     
}

void setup() {
  // initialize both serial ports:
  //Serial.begin(9600);
  //delay(1000);
  Serial1.begin(115200);
  delay(1000);
  // initialize digital pin 13-11 as an output (for LEDs)
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  // reserve 256 bytes for the inputString:
  inputString.reserve(256);
  // DHT start
  dht.begin();
}

void loop() {

  // Sensor update interval
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time of sensor reporting
    previousMillis = currentMillis;    
    Sensor_DHT ();
  }
  // Handling incoming Json from serial port 
  HandleIncommingJson (); 
  
}
