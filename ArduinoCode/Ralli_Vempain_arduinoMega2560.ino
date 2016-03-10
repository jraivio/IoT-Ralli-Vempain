// IOT RalliDroid  
// Copyright Jukka Raivio and Jussi Salonen 2016
// MIT License
// 
// Example code is based on Arduino Mega2560 
//
// This arduino demo setup is connected to esp8266 Wifi via serial. 
// The http services are managed by esp-link firmware in ESP. The esp unit is working as Wifi-Serial bridge.
// No code changes are required for the esp-link
// Github repository for esp-link https://github.com/jeelabs/esp-link

// Temperature & Humidity sensor DHT11 lib & setup
// Adafruit version, https://github.com/adafruit/DHT-sensor-library
// Json description: https://github.com/jraivio/IoT-Ralli-Vempain/wiki
#include "DHT.h"
#define DHTPIN  2       // 2 digital pin 
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


// I2C library
#include <Wire.h>
// RTC library by adafruit https://github.com/adafruit/RTClib
#include "RTClib.h"
RTC_DS1307 RTC;
String TimeStr ="";

// Multitasking functions by Arduino millis
// Initialization to enable multithreading features by using millis() functions  
// Sensor update interval
unsigned long previousMillis = 0;       // will store last time sensor updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
const long interval = 10000;            // Sensor data sending interval

// Json parser lib Copyright Benoit Blanchon 2014-2016, MIT License
// https://github.com/bblanchon/ArduinoJson
// Json description: https://github.com/jraivio/IoT-Ralli-Vempain/wiki
#include <ArduinoJson.h>
const char* command;
// Serial data handling, global variables
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
char inChar; // a char to handle incoming data

// RFID tag reader (NFC) MFRC522
/* ------------------------------------
  * RFID Card reader MFRC522 library by Miguel Balboa https://github.com/miguelbalboa/rfid
  * Signal      MFRC522      Mega 2560
  *             Pin          Pin       ´
  * -------------------------------------
  * RST/Reset   RST          49         
  * SPI SS      SDA(SS)      53       
  * SPI MOSI    MOSI         51        
  * SPI MISO    MISO         50        
  * SPI SCK     SCK          52        
*/
#include <SPI.h> // Arduino default SPI library
#include <MFRC522.h> 
#define RST_PIN   49     // Configurable, see typical pin layout above (valk)
#define SS_PIN    53    // Configurable, see typical pin layout above (vihr)
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
// Init array that will store new NUID 
byte nuidPICC[3];

/*
 * Gyroscope + accelerometer MPU-6050 library for Arduino by Jeff Rowberg
 * https://github.com/jrowberg/i2cdevlib
 * 
 */
#include <I2Cdev.h>
#include <MPU6050.h>

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// We use I2C address 0x69 as the default (0x68) is used by RTC
// Due to this AD0 pin must be pulled high
MPU6050 accelgyro(0x69);

// Global variables to store the values
int16_t ax, ay, az;
int16_t gx, gy, gz;


// Ultrasonic HC-SR04 library for Arduino by J.Rodrigo https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
// Json description: https://github.com/jraivio/IoT-Ralli-Vempain/wiki
// Max distance 51 cm
#include <Ultrasonic.h>
#define TRIG_PIN 10     // Configurable Arduino pins for HC-SR04 Trig PIN
#define ECHO_PIN 9  // Configurable Arduino pins for HC-SR04 Echo pins
Ultrasonic ultrasonic(TRIG_PIN,ECHO_PIN); 

// Edge sensor pinouts
#define right_edge 23 // Right sensor
#define left_edge 22 // Left sensor

// Motor driver Setup
// Pins L298N -> Mega board
int enA = 8;
int in1 = 4;
int in2 = 5;
int in3 = 6;
int in4 = 7;
int enB = 3;
boolean motor_active = false;
// for motor delay
unsigned long mStartMillis = 0;      // will store last time motor delay update
int course; // direction
int mspeed; // motor speed
int mdelay; // motor delay
// millis for sensor reading interval while moving, delay 500 ms
boolean sensor_moving = false;
int sensor_moving_reports = 500;
unsigned long sensor_moving_previousMillis = 0;      // will store last time light delay update

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
  float h = dht.readHumidity();  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
        // Debugging
        //Serial.println("Failed to read from DHT sensor!");
        return;
    }
    // Create Json for sensor print out
    StaticJsonBuffer<512> jsonOutBuffer;   // 514 B
    String rootJson = ""; String arrayJson = "";
    JsonObject& root = jsonOutBuffer.createObject();
    root["sensor"] = "temp_hum"; root["time"] =  TimeStr; JsonArray& array = jsonOutBuffer.createArray();
    array.add(t); array.add(h);
    // Print to Serial
    root.printTo(rootJson); array.printTo(arrayJson); String JointJson = rootJson + ":" + arrayJson + "}";
    Serial1.println(JointJson);
    return;
}

void JsonReportSensorDistance(){

  StaticJsonBuffer<512> jsonOutBuffer;   // 514 B
  String rootJson = ""; String arrayJson = "";
  JsonObject& root = jsonOutBuffer.createObject();
  root["sensor"] = "distance"; root["time"] =  TimeStr; JsonArray& array = jsonOutBuffer.createArray();
  array.add(ultrasonic.Ranging(CM)); // CM or INC
    // Debugging
    //Serial.print(ultrasonic.Ranging(CM)); // CM or INC
    //Serial.println(" cm" ); 
  root.printTo(rootJson); array.printTo(arrayJson); String JointJson = rootJson + ":" + arrayJson + "}";
    // Debugging
    //Serial.println("json string for edge:" + JointJson);
  Serial1.println(JointJson);
  return;
}  
/*
 *   Read accelerometer and gyroscope raw values and send them
 *   to ESP
 * 
 */
void JsonReportSensorAccAndGyro(){
  // read raw accel/gyro measurements from device
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Construct json
  StaticJsonBuffer<512> jsonOutBuffer;   // 514 B
  String rootJson = ""; 
  String arrayJson = "";
  JsonObject& root = jsonOutBuffer.createObject();
  
  root["sensor"] = "acc_gyro"; 
  root["time"] =  TimeStr;
  JsonArray& array = jsonOutBuffer.createArray();
  array.add( ax );
  array.add( ay );
  array.add( az );
  array.add( gx );
  array.add( gy );
  array.add( gz );
  
  root.printTo(rootJson); 
  array.printTo(arrayJson); 
  String JointJson = rootJson + ":" + arrayJson + "}";
  
  // Debuggung
  //Serial.println("json string for edge:" + JointJson);
  
  // Send to ESP
  Serial1.println(JointJson);
}

void JsonReportSensorEdge() {
   StaticJsonBuffer<512> jsonOutBuffer;   // 514 B
  String rootJson = ""; String arrayJson = "";
  JsonObject& root = jsonOutBuffer.createObject();
  root["sensor"] = "edge"; 
  root["time"] =  TimeStr;
  JsonArray& array = jsonOutBuffer.createArray();
  array.add( digitalRead(left_edge) );
  array.add( digitalRead(right_edge) );
  
  root.printTo(rootJson); array.printTo(arrayJson); String JointJson = rootJson + ":" + arrayJson + "}";
  // Debuggung
  //Serial.println("json string for edge:" + JointJson);
  Serial1.println(JointJson);
  return;
}  
void JsonReportSensorRFID() {
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
    //Serial.println(F("The NUID tag is:"));
    //Serial.print(F("In dec: "));
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

void driveMotors()
{
    // this function will run the motors across the range of possible speeds
    // note that maximum speed is determined by the motor itself and the operating voltage
    // the PWM values sent by analogWrite() are fractions of the maximum speed possible by your hardware
      // Drive Forward  
      if (course == 1) {
        digitalWrite(in1, HIGH); digitalWrite(in2, LOW); digitalWrite(in3, HIGH); digitalWrite(in4, LOW); 
        analogWrite(enA, mspeed); analogWrite(enB, mspeed);
      }
      // Drive Backward
      else if (course == 2 ) {
        digitalWrite(in1, LOW); digitalWrite(in2, HIGH); digitalWrite(in3, LOW); digitalWrite(in4, HIGH); 
        analogWrite(enA, mspeed); analogWrite(enB, mspeed);
      }
      // Turn Right, direction forward
      else if (course == 3 ) {
        digitalWrite(in1, LOW); digitalWrite(in2, LOW); digitalWrite(in3, HIGH); digitalWrite(in4, LOW); 
        analogWrite(enA, mspeed); analogWrite(enB, mspeed);
      }
      // Turn Left, direction forward
      else if (course == 4 ) {
        digitalWrite(in1, HIGH); digitalWrite(in2, LOW); digitalWrite(in3, LOW); digitalWrite(in4, LOW); 
        analogWrite(enA, mspeed); analogWrite(enB, mspeed);
      }
      // Stop
      else if (course == 0) {
        // now turn off motors
        digitalWrite(in1, LOW); digitalWrite(in2, LOW); digitalWrite(in3, LOW); digitalWrite(in4, LOW);  
      }  
}

void stopMotors() {
      // now turn off motors
    // clean up & return
    digitalWrite(in1, LOW); digitalWrite(in2, LOW); digitalWrite(in3, LOW); digitalWrite(in4, LOW);
    course = NULL;
    mspeed = NULL;
    mdelay = NULL;
    motor_active = false;
    // return;
}
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent1() {
  while (Serial1.available()> 0) {
    inChar = (char)Serial1.read(); // get the new byte: 
    inputString += inChar; // add it to the inputString:
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
        stringComplete = true;
    }
  }
}
void HandleIncomingJson() {
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
    if (command ="drive") {motor_active = true; mStartMillis = millis();}
    }
  // returning the default state of serialEvent1()
  stringComplete = false;
  // clean json incoming data buffers
  pin = NULL; value = NULL; ldelay = NULL;
  inputString="";
  inChar = NULL; JsonChar = NULL;
  return;      
}

void readTime() {

    DateTime now = RTC.now();
    String Year = String(now.year(), DEC);
    String Month = String(now.month(), DEC);
    String Day = String(now.day(), DEC);
    String Hour = String(now.hour(), DEC);
    String Minutes = String(now.minute(), DEC);
    String Seconds = String(now.second(), DEC);
    TimeStr = Year + "/" + Month + "/" + Day + ":" + Hour + ":" + Minutes + ":" + Seconds;
    // Debugging
    // Serial.print(TimeStr);
    // Serial.println();

}


void setup() {
    // initialize both serial ports:
    Serial.begin(9600); // Debugging
    delay(1000);
    Serial1.begin(115200); // Serial for esp8266
    delay(1000);
    Wire.begin(); // start I2C & RTC
    RTC.begin();
    if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running!");
      // following line sets the RTC to the date & time this sketch was compiled
      //RTC.adjust(DateTime(__DATE__, __TIME__));
    }
    // initialize digital pin 13-11 as an output (for LEDs)
    pinMode(13, OUTPUT); // Main lights leds
    pinMode(12, OUTPUT); // Direction lights
    pinMode(11, OUTPUT);
    // Egde sensor input pins
    pinMode(right_edge,INPUT); // Right sensor
    pinMode(left_edge,INPUT); // Left sensor
    inputString.reserve(256); // reserve 256 bytes for the inputString:
    dht.begin(); // Init DHT
    accelgyro.initialize(); //  ACC & Gyro initialization 
    // RFID setup
    SPI.begin(); // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522 
    for (byte i = 0; i < 6; i++) { key.keyByte[i] = 0xFF; } // RFID byte handling
    // HC-SR04 distance sensor setup (TBD, currently fixed VCC/GND)
    //pinMode(4, OUTPUT); // VCC pin
    //pinMode(7, OUTPUT); // GND ping
    //digitalWrite(4, HIGH); // VCC +5V mode  
    //digitalWrite(7, LOW);  // GND mode
}
void loop() {
    // update interval
    unsigned long currentMillis = millis();
    // DHT Sensor reporting
    if (currentMillis - previousMillis >= interval) {
      // save the last time of sensor reporting
      readTime();
      previousMillis = currentMillis;    
      JsonReportSensorDHT();
      if (motor_active == false) {
       JsonReportSensorDistance(); 
       JsonReportSensorEdge();
       JsonReportSensorAccAndGyro();
      } // slow down reporting frequency in static cases
      //JsonReportSensorACC();
    }
    if (motor_active == true) { // speed up reporting frequency in case of moving
      currentMillis = millis();
      if (currentMillis - mStartMillis >= sensor_moving_reports ) {
        readTime();
        JsonReportSensorDistance();
        JsonReportSensorEdge();
        JsonReportSensorAccAndGyro();
      }
    }
    // Look for new RFID cards
    if ( rfid.PICC_IsNewCardPresent()) { JsonReportSensorRFID(); }
    // Handling incoming Json from serial port 
    // JSon serial read, print the string when a newline arrives:
    serialEvent1(); if (stringComplete == true) { HandleIncomingJson();}
    
    // Light blinking
    if ( pin13_blinking == true){
      currentMillis = millis();
      if ( currentMillis - pin13_previousMillis >= pin13_delay ) {
          pin13_previousMillis = currentMillis; 
          if (digitalRead(13) == LOW) { digitalWrite(13, HIGH);
          } else { digitalWrite(13, LOW); }
        }
    }                       
    if ( pin12_blinking == true){
      currentMillis = millis();
      if (currentMillis - pin12_previousMillis >= pin12_delay) {
        pin12_previousMillis = currentMillis; 
        if (digitalRead(12) == LOW) { 
          digitalWrite(12, HIGH);
        } else { 
          digitalWrite(12, LOW); }
        }             
    }
    if ( pin11_blinking == true){
      currentMillis = millis();
      if ( currentMillis - pin11_previousMillis >= pin11_delay) {
        pin11_previousMillis = currentMillis; 
        if (digitalRead(11) == LOW) { 
          digitalWrite(11, HIGH);
        } else { 
          digitalWrite(11, LOW); }
        }             
    }     
    // Drive motors
    if (motor_active == true ) {
       currentMillis = millis();
       driveMotors();
       if (currentMillis - mStartMillis >= mdelay) {       
        stopMotors(); }           
    }  
 }
