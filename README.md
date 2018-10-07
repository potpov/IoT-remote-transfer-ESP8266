#IoT remote transfer ESP8266

the original application is used to remind people whenever it's time to wash dishes at home.
it uses REST api and google cloud SDK.

in this folder you can find the source code for the smart device which should be put on a sink and the bridge which is responsable for 
the comunication throught the internet.

most important features in this source code:

1. communication between the smart device and the bridge throught NRF24L01 chips

2. communication between the bridge and a remote host. using a TCP connection throught the ESP8266 chip with a custom C++ library.

APIs for the server and source code for the telegram bot which sends messages to users are available here:

https://github.com/potpov/IoT-dish-clean-telegram-bot

useful scheme of the project here.

![alt text](https://ibin.co/w800/4IITSomrVjIh.jpg)

![alt text](https://ibin.co/w800/4IITezjLIaCx.jpg)

