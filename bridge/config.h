#ifndef _CONFIG_H_
#define _CONFIG_H_

//pin arduino-shield
#define _ESP_RXPIN_ 2
#define _ESP_TXPIN_ 3

//remote server parameters
#define _HOST_NAME_ "yourhost.com"
#define _SERVER_PORT_ 80
#define _PROTOCOL_ "TCP"
#define _API_KEY_ "your secret apikey"

//local router access parameters
#define _SSID_ "your router SSID"
#define _PSW_ "your router password"

//alert time (milliseconds)
#define _ALERT1_ 65000 //a bit more than one minute
#define _ALERT2_ 125000 //a bit more than two minutes

//other parameters
#define _DELAY_ 8000

#endif //_CONFIG_H_
