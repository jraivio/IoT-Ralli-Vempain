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

// initialization to enable multithreading features by using millis() functions  
// Sensor update interval
unsigned long previousMillis = 0;       // will store last time sensor updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
const long interval = 10000;            // Sensor data sending interval

  
// Json parser lib 
// Copyright Benoit Blanchon 2014-2016
// MIT License
//
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson
// If you like this project, please add a star!
#include <ArduinoJson.h>
const char* command;

/* e.g json format {"command":"drive","mdata":[1,150,500]}
  * where course is forward, speed value is 150 and 500 ms driving period
  * Speed range 100 - 255 is recommended for this setup
  * e.g curl via esp-link >curl -i -X POST  http://192.168.0.171/console/send?text=%7B%22command%22%3A%22drive%22%2C%22mdata%22%3A%5B1%2C150%2C500%5D%7D
  * Example json formats 
  * drive forward {"command":"drive","mdata":[1,100,500]}
  * drive backward {"command":"drive","mdata":[2,100,500]}
  * turn right, direction forward {"command":"drive","mdata":[1,100,500]}
  * turn left, direction forward {"command":"drive","mdata":[1,100,500]}
  * stop with delay, 5 sec {"command":"drive","mdata":[0,0,5000]}
  * full stop {"command":"drive","mdata":[,,]}
  * Control LED digital output pins  
  * e.g pin 12 on, not blinking {"command":"lights","data":[12,1,0]}
  * e.g curl via esp-link >curl -i -X POST http://192.168.0.171/console/send?text=%7B%22command%22%3A%22lights%22%2C%22data%22%3A%5B12%2C1%2C0%5D%7D
  * UTF-8 encoder tool http://meyerweb.com/eric/tools/dencoder/
  * e.g pin 12 on, blinking with 500 ms delay {"command":"lights","data":[12,1,500]}
  * e.g curl via esp-link >curl -i -X POST http://192.168.0.171/console/send?text=%7B%22command%22%3A%22lights%22%2C%22data%22%3A%5B12%2C1%2C500%5D%7D
*/

/* ------------------------------------
  * RFID Card reader MFRC522
  * By Miguel Balboa
  * https://github.com/miguelbalboa/rfid
  * Signal      MFRC522      Mega 2560
  *             Pin          Pin       ´
  * -------------------------------------
  * RST/Reset   RST          49         
  * SPI SS      SDA(SS)      53       
  * SPI MOSI    MOSI         51        
  * SPI MISO    MISO         50        
  * SPI SCK     SCK          52        
*/

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   49     // Configurable, see typical pin layout above (valk)
#define SS_PIN    53    // Configurable, see typical pin layout above (vihr)
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[3];


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
boolean motor_active = false;
// for motor delay
unsigned long mpreviousMillis = 0;      // will store last time motor delay update
int course; // direction
int mspeed; // motor speed
int mdelay; // motor delay

// light blinking 
boolean pin13_blinking = false;
boolean pin12_blinking = false;
boolean pin11_blinking = false;
int pin13_delay;
int pin12_delay;
int pin11_delay;
unsigned long pin13_previousMillis = 0;      // will store last time light delay update
unsigned long pin12_previousMillis = 0;      // will store last time light delay update
unsigned long pin11_previousMillis = 0;      // will store last time light delay update

void JsonReportSensorDHT() {

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

void JsonReportSensorDistance(){}  // TBD
void JsonReportSensorACC(){}       // TBD
void JsonReportSensorEdge() {}     // TBD
void JsonReportSensorRFID() {

    // Verify if the NUID has been readed
    if ( ! rfid.PICC_ReadCardSerial())
      return;
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    // Check is the PICC of Classic MIFARE type
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      return;
    }

    if (rfid.uid.uidByte[0] != nuidPICC[0] || 
      rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || 
      rfid.uid.uidByte[3] != nuidPICC[3] ) {
      Serial.println(F("A new card has been detected."));
      // Define Json for sensor print out
      StaticJsonBuffer<512> jsonOutBuffer;   // 514 B
      String rootJson = "";
      String arrayJson = "";
      JsonObject& root = jsonOutBuffer.createObject();
      root["sensor"] = "rfid";
      JsonArray& array = jsonOutBuffer.createArray();
      // Store NUID into nuidPICC array
      for (byte i = 0; i < 4; i++) {
        nuidPICC[i] = rfid.uid.uidByte[i];
        }   
      Serial.println(F("The NUID tag is:"));
      Serial.print(F("In dec: "));
      byte *buffer = rfid.uid.uidByte; 
      byte bufferSize = rfid.uid.size;
      String NUID = "";
      for (byte i = 0; i < bufferSize; i++) {
          //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
          //Serial.print(buffer[i], DEC);
          NUID = String(NUID + String(buffer[i], DEC)); 
          }
      array.add(NUID);
      // Print json string to Serial1
      root.printTo(rootJson);
      array.printTo(arrayJson);
      String JointJson = rootJson + ":" + arrayJson + "}";
      //Serial.println("json string for rfid:" + JointJson);
      Serial1.println(JointJson);
      }
    else Serial.println(F("Card read previously."));
   
    rfid.PICC_HaltA(); // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption on PCD
    return;  
    
}

//Helper routine to dump a byte array as dec values to Serial.

void Motor()
{
    // this function will run the motors across the range of possible speeds
    // note that maximum speed is determined by the motor itself and the operating voltage
    // the PWM values sent by analogWrite() are fractions of the maximum speed possible by your hardware
    // Drive Forward
    if (course == 1) {
        digitalWrite(in1, HIGH); digitalWrite(in2, LOW); digitalWrite(in3, HIGH); digitalWrite(in4, LOW); 
        analogWrite(enA, mspeed); analogWrite(enB, mspeed);
        delay(mdelay);
    }
    // Drive Backward
    else if (course == 2) {
        digitalWrite(in1, LOW); digitalWrite(in2, HIGH); digitalWrite(in3, LOW); digitalWrite(in4, HIGH); 
        analogWrite(enA, mspeed); analogWrite(enB, mspeed);
        delay(mdelay);
    }
    // Turn Right, direction forward
    else if (course == 3) {
        digitalWrite(in1, LOW); digitalWrite(in2, LOW); digitalWrite(in3, HIGH); digitalWrite(in4, LOW); 
        analogWrite(enA, mspeed); analogWrite(enB, mspeed);
        delay(mdelay);
    }
    // Turn Left, direction forward
    else if (course == 4) {
        digitalWrite(in1, HIGH); digitalWrite(in2, LOW); digitalWrite(in3, LOW); digitalWrite(in4, LOW); 
        analogWrite(enA, mspeed); analogWrite(enB, mspeed);
        delay(mdelay);
    }
    // Stop with delay
    else if (course == 0) {
        // now turn off motors
        digitalWrite(in1, LOW); digitalWrite(in2, LOW); digitalWrite(in3, LOW); digitalWrite(in4, LOW);  
        delay(mdelay);
    }
    // now turn off motors
    // clean up & return
    digitalWrite(in1, LOW); digitalWrite(in2, LOW); digitalWrite(in3, LOW); digitalWrite(in4, LOW);
    course = NULL;
    mspeed = NULL;
    mdelay = NULL;
    motor_active = false;
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


void HandleIncommingJson() {

      StaticJsonBuffer<512> jsonInBuffer;                 
      const char *JsonChar = inputString.c_str(); // 1 KB
      JsonObject& root = jsonInBuffer.parseObject(JsonChar);
      int pin;
      int value;
      int ldelay;
      // Verify Json 
      if (JsonChar!=NULL && !root.success()) {
        Serial.println("parseObject() failed: ");
        Serial.println(JsonChar);
      }
      else {
        // Led pins 13-11
        command = root["command"];
        pin = root["data"][0];
        value = root["data"][1];
        ldelay = root["data"][2];
        course = root["mdata"][0];
        mspeed = root["mdata"][1];
        mdelay = root["mdata"][2];
        if (command="lights") {
            // Lights on, not blinking
            if (value == 1 && (ldelay == 0 || ldelay == NULL)){
              digitalWrite(pin, HIGH);
              if (pin == 13) { pin13_blinking = false; }
              if (pin == 12) { pin12_blinking = false; }   
              if (pin == 11) { pin11_blinking = false; }                
            }
            // blinking lights
            else if (value == 1 && (!ldelay == 0 || !ldelay == NULL)){
              if (pin == 13) { pin13_blinking = true; pin13_delay = ldelay;}
              if (pin == 12) { pin12_blinking = true; pin12_delay = ldelay;}   
              if (pin == 11) { pin11_blinking = true; pin11_delay = ldelay;}              
            }
            // Lights off
            else if (value == 0 ){
              digitalWrite(pin, LOW);
              if (pin == 13) { pin13_blinking = false; }
              if (pin == 12) { pin12_blinking = false; }   
              if (pin == 11) { pin11_blinking = false; }  
            }
        }
        if (command ="drive") {motor_active = true; }
      }
    // returning the default state of serialEvent1()
    stringComplete = false;
    // clean json incoming data buffers
    pin = NULL; value = NULL; ldelay = NULL;
    inputString="";
    inChar = NULL; JsonChar = NULL;
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
    inputString.reserve(256); // reserve 256 bytes for the inputString:
    dht.begin(); // Init DHT
    SPI.begin(); // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522 
    for (byte i = 0; i < 6; i++) { key.keyByte[i] = 0xFF; } // RFID byte handling
}

void loop() {
    // update interval
    unsigned long currentMillis = millis();
    unsigned long pin13_currentMillis = millis();
    unsigned long pin12_currentMillis = millis();
    unsigned long pin11_currentMillis = millis();
    unsigned long mcurrentMillis = millis();
    // Sensors
    if (currentMillis - previousMillis >= interval) {
      // save the last time of sensor reporting
      previousMillis = currentMillis;    
      JsonReportSensorDHT();
      //JsonReportSensorDistance();
      //JsonReportSensorACC();
      //JsonReportSensorEdge();
    }
    // Look for new RFID cards
    if ( rfid.PICC_IsNewCardPresent()) { JsonReportSensorRFID(); }
     
    // Handling incoming Json from serial port 
    // JSon serial read
    // print the string when a newline arrives:
    serialEvent1(); if (stringComplete == true) { HandleIncommingJson();}
    // Light blinking
    if ( pin13_blinking == true){
        if (pin13_currentMillis - pin13_previousMillis >= pin13_delay) {
          pin13_previousMillis = pin13_currentMillis; 
          if (digitalRead(13) == LOW) { digitalWrite(13, HIGH);
          } else { digitalWrite(13, LOW); }
        }
    }                       
    if ( pin12_blinking == true){
        if (pin12_currentMillis - pin12_previousMillis >= pin12_delay) {
          pin12_previousMillis = pin12_currentMillis; 
          if (digitalRead(12) == LOW) { digitalWrite(12, HIGH);
          } else { digitalWrite(12, LOW); }
        }             
    }
    if ( pin11_blinking == true){
        if (pin11_currentMillis - pin11_previousMillis >= pin11_delay) {
          pin11_previousMillis = pin11_currentMillis; 
          if (digitalRead(11) == LOW) { digitalWrite(11, HIGH);
          } else { digitalWrite(11, LOW); }
        }             
    }     
    // Start engine
    // TODO motor function interrupts still all other functions
    if (motor_active == true ) { 
        if (mcurrentMillis - mpreviousMillis >= mdelay) {
          mpreviousMillis = mcurrentMillis;         
          Motor(); }
    }  
 }
