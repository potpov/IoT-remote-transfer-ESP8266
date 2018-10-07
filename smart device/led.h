#ifndef __LED_H__
#define __LED_H__
#include <Arduino.h>
//led configuration
#define SH_CP A5  //attach to A5 
#define ST_CP A4  //attach to A4 
#define DS    A3  // attach to A3

class Led {   
   void Write(unsigned char dat) {
      digitalWrite(ST_CP,LOW);//write a LOW value to ST_CP
      for(int i = 0; i < 8; i++) {
          digitalWrite(SH_CP,LOW);//write a LOW value to SH_CP 
          if(dat & 0x80) //if the highest bits of dat is 1
              digitalWrite(DS,HIGH);//write a HIGH value to DS
          else
              digitalWrite(DS,LOW);//write a LOW value to DS
          asm("nop");//do-nothing operation
          asm("nop");
          dat <<= 1;//shiftleft one bit
          digitalWrite(SH_CP,HIGH);//write a HIGH value to SH_CP    
      }
      digitalWrite(ST_CP,HIGH);//write a HIGH value to ST_CP
      asm("nop");
      asm("nop");
      digitalWrite(ST_CP,LOW);//write a LOW value to ST_CP
  } 
   
  public:    
    Led(){
      digitalWrite(ST_CP,LOW);
      digitalWrite(DS,LOW);
      digitalWrite(SH_CP,LOW);
      //configures ST_CP,DS,SH_CP of 74hc595 as OUTPUT
      pinMode(SH_CP,OUTPUT);
      pinMode(DS,OUTPUT);
      pinMode(ST_CP,OUTPUT);
    }
    void fibonacci(void){
      unsigned char sequence[6] = {0x01,0x01,0x03,0x07,0x1F,0xFF};
      for(int i = 0; i < 6; i++) {
        this->Write(~sequence[i]);
        delay(200);
      }
    }
    void off(){
      this->Write(~0x00);
    }
};
#endif
