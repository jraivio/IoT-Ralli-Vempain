###Eye Candy: Ralli-Vempain droid equipped with all bells and whistles

![alt tag] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Pictures/Ralli-Vempain-layout.jpg)

_Note that layout description is overal. Some late findings... 6V battery is not enough. 7V-12V is recommended._    
_Not up-to-dated picture. Large number of sensors are missing_     

###IOT Droid wiring diagram by Fritzing tool:
![alt tag] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Pictures/Ralli-Vempain-Fritzing-wiring.png)
_Note that layout description is overal. L298N requires additional control pinouts. L298N enA-> Arduino pin8 and L298N enB-> Arduino pin3._    


Wiring diagram is designed by [Fritzing tool.](http://fritzing.org/home/) The source code of the above wiring diagram is available also in this [IOT-Ralli-Vempain repository](https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Rally_Droid2.fzz)

###List of selected hardware:
- [Microcontroller: Arduino Mega2560] (https://www.arduino.cc/en/Main/ArduinoBoardMega2560)
- [H-Bridge motor driver: L298N] (http://www.geeetech.com/wiki/index.php/L298N_Motor_Driver_Board)
- [Distance sensor: HC-SR04] (http://www.electroschematics.com/8902/hc-sr04-datasheet/)
- [Temperature and Humidity sensor: DHT11] (http://www.micropik.com/PDF/dht11.pdf)
- [RFID reader/writer: Mifare 13.56Mhz RC522] (http://www.nxp.com/documents/data_sheet/MFRC522.pdf)
- [Infrared line track Sensor: TCRT5000] (http://www.aliexpress.com/item/Sensor-For-Arduino-LM393-IR-Infrared-Line-Tracking-Transducer-TCRT5000-Obstacle-Avoidance-Module-Board-Photoelectric-Switch/32591217818.html)
- [IOT Wifi-serial bridge: ESP8266-01] (https://www.adafruit.com/images/product-files/2471/0A-ESP8266__Datasheet__EN_v4.3.pdf)
- [Frame, Wheels and DC motor] (http://www.aliexpress.com/item/New-Motor-Smart-Robot-Car-Chassis-Kit-Speed-Encoder-Battery-Box-For-Arduino-Free-Shipping/1924502066.html)
- [Battery 2 x 3.7V: 18650] (https://www.reddit.com/r/flashlight/wiki/18650)
- [400 Point Breadboard] (http://www.aliexpress.com/item/400-Tie-Point-Interlocking-Solderless-mini-Breadboard-Free-Shipping-Dropshipping/1248613686.html)
- [3.3v regulator: AMS1117] (http://www.aliexpress.com/item/1pc-DC-DC-AMS1117-1-2V-Step-Down-Voltage-Regulator-Adapter-Convertor-2P-Single-Row-Hot/32369895833.html)


###esp8266 - esp-link:    
![alt tag] (http://www.partco.biz/verkkokauppa/images/tuotekuvat/e/esp8266_esp-01_top-medium.jpg)

This demo setup is based on esp8266-01 Serial-Wifi bridge. The default AT based firmware of esp8266 has replaced by the esp-link version.

esp-link serial bridge description:    
https://github.com/jeelabs/esp-link#serial-bridge-and-connections-to-arduino-avr-arm-lpc-microcontrollers

esp-link firmware releases and flashing instructions for esp8266:     
https://github.com/jeelabs/esp-link#releases-downloads

###esp-link user interface:   
esp-link comes with very handy user interface. It is possible to configure and admistrate Wifi parameters, managing MQTT/REST parameters and also handling serial events from Arduino by the Web Browser. The UI allows to send and receive serial events. This feature makes life much easier in case of troubleshooting.    

esp-link web UI. example of μC Console:    
![alt tag] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Pictures/esp-link.png)    



###esp8266-01 pinout:   
![alt tag] (http://www.partco.biz/verkkokauppa/images/tuotekuvat/e/esp8266_esp-01_pinout-large.jpg)


