#include <Arduino.h>
#include <SoftwareSerial.h>
#include "config.h"

SoftwareSerial esp(_ESP_RXPIN_, _ESP_TXPIN_);

class Http {

  bool syncCommands(){
    Serial.println("SYNC STARTS..");
    unsigned int timeout = millis();
    bool ACK = false;
    while(millis() - timeout < _DELAY_){ //using millis instead of delay to avoid buffer overflow
      if(esp.available()>0) {
        auto result = esp.readString(); //flush the buffer in any case
        if(result.indexOf("OK")>0)  //error checking
          ACK = true;
        Serial.println("wifi board: " + result);
      }
    }
    esp.flush();  //clear the buffer
    return ACK;
  }

  void hostConnect(String hostname=_HOST_NAME_, unsigned int port=_SERVER_PORT_, String protocol=_PROTOCOL_){
    Serial.println("HOST CONNECT STARTS..");
    String cmd = String( "AT+CIPSTART=\"" + protocol + "\",\"" + hostname + "\"," + port + "\r\n");
    do {
      esp.write(cmd.c_str());
    }
    while(this->syncCommands());   
  }

  void hostDisconnect(){
    esp.write("AT+CIPCLOSE\r\n");
    this->syncCommands();
  }
  
  public:

    Http(){
      esp.begin(115200);
      esp.flush();
    }

    void test(){
      esp.write("AT\r\n");
      this->syncCommands();
    }

    void clearBuffer(){
      delay(1000);
      esp.flush();
    }

    void login(String ssid=_SSID_, String psw=_PSW_){
      String cmd = String( "AT+CWJAP=\"" + ssid + "\",\"" + psw + "\"\r\n");
      esp.write(cmd.c_str());
      this->syncCommands();
    }

    void sendData(String path, String data, String host=_HOST_NAME_){  
        Serial.println("SEND DATA STARTS..");     
        this->hostConnect(); //opening TCP connection to the server
        String packet = "POST ";
        packet += path;
        packet += " HTTP/1.1";
        packet += "\nHost: ";
        packet += _HOST_NAME_;
        packet += "\nCache-Control: no-cache";
        packet += "\nContent-Type: application/x-www-form-urlencoded";
        packet += "\nContent-Length: ";
        packet += data.length();
        packet += "\n\n";
        packet += data;  
        //sending package
        String sender = "AT+CIPSEND=";
        sender += packet.length();
        sender += "\r\n";
        esp.write(sender.c_str());
        this->syncCommands();
        esp.write(packet.c_str());
        this->syncCommands();
        //closing TCP connection
        this->hostDisconnect();
    }

    void newJob(){
        String data;
        data = "secret=";
        data += _API_KEY_;
        this->sendData("/api/newjob", data);
    }

    void jobComplete(){
        String data;
        data = "secret=";
        data += _API_KEY_;
        data += "&user=Unknown";
        this->sendData("/api/jobcomplete", data);
    }

    void alert(unsigned long timePast){
        String data;
        data = "secret=";
        data += _API_KEY_;
        data += "&time=";
        data += timePast;
        this->sendData("/api/alert", data);
    }
};
