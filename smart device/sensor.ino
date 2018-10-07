#include "led.h"
#include "trasmitter.h"
/* button inline setup */
#define    PIN_KEY    2

/* trasmitter inline setup */
#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  3   // 3 unsigned chars TX payload
unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = { 0x34,0x43,0x10,0x10,0x01 }; 
unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

/*program starts*/
Led myled;
bool on;

void setup() {
  on = 0;
  pinMode(PIN_KEY, INPUT);
  digitalWrite(PIN_KEY, HIGH);
  tx_buf[0] = 0x00;
  tx_buf[1] = 0x00;
  tx_buf[2] = 0x00;
  myled.off();
  NRF_Init();  
}

void loop() {
    if(digitalRead(PIN_KEY) == 0){ //the key pressed
      delay(20);  //disappears shakes
      NRF_SeTxMode(); //ready to send
      if(on) { //turning off... sending 0...
        do {
          tx_buf[0] = 0x00; 
          NRF_Send(tx_buf);
        }
        while(!NRF_CheckAck());
        myled.off();
      }
      else { //turning on.. sending 1...
        do { 
          tx_buf[0] = 0x01;
          NRF_Send(tx_buf);
        }
        while(!NRF_CheckAck());
        myled.fibonacci();
      }
      on = not on; //switch status
      while(!digitalRead(PIN_KEY));
    }
}
