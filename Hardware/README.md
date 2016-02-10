###Ralli-Vempain droid equipped with all bells and whistles

![alt tag] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Pictures/Ralli-Vempain-layout.jpg)

_Note that layout description is overal. Some late findings... 6V battery is not enough. 7V-12V is recommended._    

**IOT Droid wiring diagram by Fritzing tool:**
![alt tag] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Pictures/Ralli-Vempain-Fritzing-wiring.png)
_Note that layout description is overal. L298N requires additional control pinouts. L298N enA-> Arduino pin8 and L298N enB-> Arduino pin3._    

Wiring diagram is designed by [Fritzing tool.](http://fritzing.org/home/) The source code of the above wiring diagram is available also in this [IOT-Ralli-Vempain repository](https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Rally_Droid2.fzz)




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


