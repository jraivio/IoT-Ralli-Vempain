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
#define DHTPIN  2       // 2 digital pin we're connected to
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

// Motor Setup
int enA = 8;
int in1 = 4;
int in2 = 5;
int in3 = 6;
int in4 = 7;
int enB = 3;


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

void Motor_Forward(int mspeed, int mdelay)
{
  // this function will run the motors across the range of possible speeds
  // note that maximum speed is determined by the motor itself and the operating voltage
  // the PWM values sent by analogWrite() are fractions of the maximum speed possible 
  // by your hardware
  // turn on motors
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW); 
  analogWrite(enA, mspeed);
  analogWrite(enB, mspeed);
  delay(mdelay);
  // now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);  
}

void Motor_Backward(int mspeed,int mdelay)
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH); 
  analogWrite(enA, mspeed);
  analogWrite(enB, mspeed);
  delay(mdelay);
  // now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);  
}

void Motor_Right(int mspeed,int mdelay)
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW); 
  analogWrite(enA, mspeed);
  analogWrite(enB, mspeed);
  delay(mdelay);
  // now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);  
}

void Motor_Left(int mspeed,int mdelay)
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW); 
  analogWrite(enA, mspeed);
  analogWrite(enB, mspeed);
  delay(mdelay);
  // now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);  
}

void Motor_Stop()
{
  // now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);  
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
    // read from json

    const char* command = root["command"];
    int pin = root["data"][0];
    int value = root["data"][1];
    int course = root["mdata"][0];
    int mspeed = root["mdata"][1];
    int mdelay = root["mdata"][2];

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
          // UFT-8 encoder support http://meyerweb.com/eric/tools/dencoder/
          else if (value == 0){
            digitalWrite(pin, LOW);
          }
    }
    
    // e.g json format {"command":"drive","mdata":[1,150,500]}
    // where course is forward, speed value is 150 and 500 ms driving period
    // Speed range 100 - 255 is recommended for this setup
    // e.g curl via esp-link >curl -i -X POST  http://192.168.0.171/console/send?text=%7B%22command%22%3A%22drive%22%2C%22mdata%22%3A%5B1%2C150%2C500%5D%7D
    if (command="drive") { 
          // forward {"command":"drive","mdata":[1,100,500]}
          if (course == 1){
          Motor_Forward(mspeed,mdelay);
          }
          // backward {"command":"drive","mdata":[2,100,500]}
          else if (course == 2){
          Motor_Backward(mspeed,mdelay);
          }
          // right {"command":"drive","mdata":[3,100,500]}
          else if (course == 3){
          Motor_Right(mspeed,mdelay);
          }
          // left {"command":"drive","mdata":[4,100,500]}
          else if (course == 4){
          Motor_Left(mspeed,mdelay);
          }
          // stop {"command":"drive","mdata":[0]}     
          else if (course == 0){
          Motor_Stop();
          }   
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
    Serial.begin(9600);
    delay(1000);
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
