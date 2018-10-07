#include <Arduino.h>
#include <SoftwareSerial.h>
#include "config.h"
#include "http.h"
#include "receiver.h"

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  3  // 32 unsigned chars TX payload
unsigned char RX_ADDRESS[TX_ADR_WIDTH]  = { 0x34,0x43,0x10,0x10,0x01 };
unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; //using just the 0-th byte.
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

Http client; //boot wifi trasmitter
bool dirty;
//variables for reminders
unsigned long lastAlert;
bool alert1Flag = false;
bool alert2Flag = false;

void setup() {
  Serial.begin(115200);
  NRF_Init();           // boot receiver mode
  dirty = 0;            //in the beginning was the word, without dirty dishes. [god]
  lastAlert = millis();  //init lastAlert but unused till dirty is set false.
  client.login();
}

void loop() {

    //events listener
    NRF_SetRxMode();
    if(NRF_Receive(rx_buf))
    {
      if(rx_buf[0] == 0 && dirty == 1){ //dishes have been clean
        Serial.println("piatti puliti. lo comunico");
        client.jobComplete();
        dirty = not dirty;
        lastAlert = millis();
      }
      else if(rx_buf[0] == 1 && dirty == 0){ //there's work to do son
        Serial.println("piatti da pulire. lo comunico");
        dirty = not dirty;
        client.newJob();
      }
      else
        Serial.println("incongruenza status");
      rx_buf[0] = 0;
      client.clearBuffer();
      Serial.println("\n\nsystem ready.");
    }
    delay(500);

    
    //first reminder
    if(dirty && (millis() - lastAlert) > _ALERT1_ && !alert1Flag){
      Serial.println("mando primo sollecito");
      client.alert(millis() - lastAlert);
      alert1Flag = not alert1Flag;
    }

    
    //second reminder
    if(dirty && (millis() - lastAlert) > _ALERT2_ && !alert2Flag){
      Serial.println("mando secondo sollecito");
      client.alert(millis() - lastAlert);   
      alert2Flag = not alert2Flag;
    }
}
