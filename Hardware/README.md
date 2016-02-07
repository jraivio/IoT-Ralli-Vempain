**Hardware layout are described below:**
![alt tag] (http://ibin.co/2TrR4RWtmNH5)

_Note that layout description is overal. Some late findings... 6V battery is not enough. 7V-12V is recommended._    

**Arduino wiring setups are described below:**
![alt tag] (http://ibin.co/2TrT88krOaqu)
_Note that layout description is overal. L298N requires additional control pinouts. L298N enA-> Arduino pin8 and L298N enB-> Arduino pin3._    

Wiring diagram is designed by [Fritzing tool.](http://fritzing.org/home/) The source code of this project diagram is available in this [repository](https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Rally_Droid2.fzz)




###esp8266 - esp-link:    
![alt tag] (http://www.partco.biz/verkkokauppa/images/tuotekuvat/e/esp8266_esp-01_top-medium.jpg)

This demo setup is based on esp8266-01 Serial-Wifi bridge. The default AT based firmware of esp8266 has replaced for the esp-link version.

esp-link serial bridge description:    
https://github.com/jeelabs/esp-link#serial-bridge-and-connections-to-arduino-avr-arm-lpc-microcontrollers

esp-link firmware releases and flashing instructions for esp8266:     
https://github.com/jeelabs/esp-link#releases-downloads

###esp-link user interface:   
esp-link comes with very handy user interface. It is possible to configure and admistrate Wifi parameters, managing MQTT/REST parameters and also handling serial events from Arduino. The UI allows to send and receive events.    

More details in below picture:    
![alt tag] (https://github.com/jraivio/IoT-Ralli-Vempain/blob/master/Hardware/Pictures/esp-link.png)    



###esp8266 pinout:   
![alt tag] (http://www.partco.biz/verkkokauppa/images/tuotekuvat/e/esp8266_esp-01_pinout-large.jpg)


