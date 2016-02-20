

###High level SW architecture:
![alt tag] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Pictures/Ralli_Vempain_highlevel_SW_arc.png)

[Arduino sample code for Ralli-Vempain gadget] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/ArduinoCode/Ralli_Vempain_arduinoMega2560.ino)

**Getting started with Arduino SW development environment:**    
- [Download IDE] (https://www.arduino.cc/en/Main/Software)    
- [Instructions] (https://www.arduino.cc/en/Guide/Environment)     

**Following 3rd party libraries in use:**   
- [Temperature & Humidity sensor DHT11, Adafruit version] (https://github.com/adafruit/DHT-sensor-library)     
- [JSON string parser, by Benoit Blanchon] (https://github.com/bblanchon/ArduinoJson)    
- [RFID Card reader MFRC522, by Miguel Balboa] (https://github.com/miguelbalboa/rfid)    

**Following functions in use:**  
- `JsonReportSensorDHT()`: // DHT11 sensor data reader & json report creation    
- `JsonReportSensorRFID()`: // RFID card reader sensor & json report creation    
- `Motor()`:  // Motor control functions, adapted for L298N    
- `serialEvent1()`: // Monitoring new serial events from esp  
- `HandleIncommingJson()`: // json parser for incoming commands from esp    
- `setup()`: // Standard Arduino function to setup the system    
- `loop()`: // Standard Arduino loop function. Primary function to call other functions.   

_Note that details of json string formats are described in the [wiki part] (https://github.com/jraivio/IoT-Ralli-Vempain/wiki)_  



