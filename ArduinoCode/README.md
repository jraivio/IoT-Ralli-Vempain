

###High level SW architecture:
![alt tag] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Pictures/Ralli_Vempain_highlevel_SW_arc.png)

**Following libraries in use:**   
Temperature & Humidity sensor DHT11, Adafruit version:    
https://github.com/adafruit/DHT-sensor-library    

json string parser library, by Benoit Blanchon:    
https://github.com/bblanchon/ArduinoJson

**Following functions in use:**  
`JsonReportSensorDHT()`: // DHT11 sensor data reader & json report creation    
`Motor()`:  // Motor control functions, adapted for L298N    
`serialEvent1()`: // Monitoring new serial events from esp  
`HandleIncommingJson()`: // json parser for incoming commands from esp    
`setup()`: // Standard Arduino function to setup the system    
`loop()`: // Standard Arduino loop function. Primary function to call other functions.   

Note that details of json string formats are described in the wiki part  
https://github.com/jraivio/IoT-Ralli-Vempain/wiki
