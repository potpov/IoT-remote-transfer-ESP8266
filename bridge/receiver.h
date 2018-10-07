#ifndef _RECEIVER_H__
#define _RECEIVER_H__

#include <Arduino.h>

//----------------------------------------------------------------------------------------------
// SPI(nRF24L01) commands
#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register
//----------------------------------------------------------------------------------------------
#define RX_DR    0x40
#define TX_DS    0x20
#define MAX_RT   0x10
//----------------------------------------------------------------------------------------------
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
#define TX_ADR_WIDTH    5  // 5 unsigned chars TX(RX) address width   
#define TX_PLOAD_WIDTH  3  // 3 unsigned chars TX payload 
//----------------------------------------------------------------------------------------------
#define CE           9   // Chip Enable Activates RX or TX mode     
#define CSN          10  // SPI Chip Select ,Slave Enable signal, controlled by master
#define SCK_PIN      13  // SPI Clock,controlled by master   
#define MOSI_PIN     11  // SPI Master Data Output,Slave Data Input    
#define MISO_PIN     12  // SPI Master Data Input, Slave Data Output, with tri-state option
#define IRQ          8   // Maskable interrupt pin
//----------------------------------------------------------------------------------------------

//-------------------------------------private function-----------------------------------------
extern unsigned char SPI_RW(unsigned char Byte);
extern unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value);
extern unsigned char SPI_Read(unsigned char reg);
extern unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);
extern unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);
//----------------------------------------------------------------------------------------------

//--------------------------------------public function-----------------------------------------
extern void NRF_Init(void);
extern void NRF_SeTxMode(void);
extern void NRF_Send(unsigned char *buf_tx);
extern unsigned char NRF_CheckAck(void);
extern void NRF_SetRxMode(void);
extern unsigned char NRF_Receive(unsigned char *buf_rx);
//----------------------------------------------------------------------------------------------


extern unsigned char TX_ADDRESS[TX_ADR_WIDTH];
extern unsigned char RX_ADDRESS[TX_ADR_WIDTH];
extern unsigned char rx_buf[TX_PLOAD_WIDTH]; 
extern unsigned char tx_buf[TX_PLOAD_WIDTH];

//----------------------------------------------------------------------------------------------
// Function: init_io();
// Description:
// flash led one time,chip enable(ready to TX or RX Mode),
// Spi disable,Spi clock line init high
//----------------------------------------------------------------------------------------------
void NRF_Init(void)
{
    pinMode(CE,  OUTPUT);
    pinMode(SCK_PIN, OUTPUT);
    pinMode(CSN, OUTPUT);
    pinMode(MOSI_PIN,  OUTPUT);
    pinMode(MISO_PIN, INPUT);
    pinMode(IRQ, INPUT);
    digitalWrite(IRQ, 0);
    digitalWrite(CE, 0);    // chip enable
    digitalWrite(CSN, 1);   // Spi disable	
}

//----------------------------------------------------------------------------------------------
// Function: SPI_RW();
// 
// Description:
// Writes one unsigned char to nRF24L01, and return the unsigned char read
// from nRF24L01 during write, according to SPI protocol
//----------------------------------------------------------------------------------------------
unsigned char SPI_RW(unsigned char Byte)
{
    unsigned char i;
    for(i=0;i<8;i++)    // output 8-bit
    {
        if(Byte&0x80)
        {
            digitalWrite(MOSI_PIN, 1);
        }
        else
        {
            digitalWrite(MOSI_PIN, 0);
        }
        digitalWrite(SCK_PIN, 1);
        Byte <<= 1;    // shift next bit into MSB..
        if(digitalRead(MISO_PIN) == 1)
        {
            Byte |= 1;    // capture current MISO bit
        }
        digitalWrite(SCK_PIN, 0);
    }
    return(Byte);    // return read unsigned char
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Function: SPI_RW_Reg();
// 
// Description:
// Writes value 'value' to register 'reg'
//----------------------------------------------------------------------------------------------
unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value)
{
    unsigned char status;
    digitalWrite(CSN, 0);    // CSN low, init SPI transaction
    status = SPI_RW(reg);    // select register
    SPI_RW(value);           // and write value to it..
    digitalWrite(CSN, 1);    // CSN high again
    return(status);          // return nRF24L01 status unsigned char
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Function: SPI_Read();
// 
// Description:
// Read one unsigned char from nRF24L01 register, 'reg'
//----------------------------------------------------------------------------------------------
unsigned char SPI_Read(unsigned char reg)
{
    unsigned char reg_val;
    digitalWrite(CSN, 0);    // CSN low, initialize SPI communication...
    SPI_RW(reg);             // Select register to read from..
    reg_val = SPI_RW(0);     // ..then read register value
    digitalWrite(CSN, 1);    // CSN high, terminate SPI communication 
    return(reg_val);         // return register value
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Function: SPI_Read_Buf();
// 
// Description:
// Reads 'unsigned chars' #of unsigned chars from register 'reg'
// Typically used to read RX payload, Rx/Tx address
//----------------------------------------------------------------------------------------------
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
    unsigned char status,i;
    digitalWrite(CSN, 0);       // Set CSN low, init SPI tranaction
    status = SPI_RW(reg);       // Select register to write to and read status unsigned char
    for(i=0;i<bytes;i++)
    {
        pBuf[i] = SPI_RW(0);    // Perform SPI_RW to read unsigned char from nRF24L01
    }
    digitalWrite(CSN, 1);       // Set CSN high again
    return(status);             // return nRF24L01 status unsigned char
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Function: SPI_Write_Buf();
// 
// Description:
// Writes contents of buffer '*pBuf' to nRF24L01
// Typically used to write TX payload, Rx/Tx address
//----------------------------------------------------------------------------------------------
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
    unsigned char status,i;
    digitalWrite(CSN, 0);    // Set CSN low, init SPI tranaction
    status = SPI_RW(reg);    // Select register to write to and read status unsigned char
    for(i=0;i<bytes; i++)    // then write all unsigned char in buffer(*pBuf)
    {
        SPI_RW(*pBuf++);
    }
    digitalWrite(CSN, 1);    // Set CSN high again
    return(status);          // return nRF24L01 status unsigned char
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Function: TX_Mode();
// 
// Description:
// This function initializes one nRF24L01 device to
// TX mode, set TX address, set RX address for auto.ack,
// fill TX payload, select RF channel, datarate & TX pwr.
// PWR_UP is set, CRC(2 unsigned chars) is enabled, & PRIM:TX.
// 
// ToDo: One high pulse(>10us) on CE will now send this
// packet and expext an acknowledgment from the RX device.
//----------------------------------------------------------------------------------------------
void NRF_SeTxMode(void)
{
    digitalWrite(CE, 0);
    SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
    SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0  0x01
    SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0 0x01
    SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1A); // 500us + 86us, 10 retrans... 0x1a
    SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
    SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
    SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 unsigned chars) & Prim:TX. MAX_RT & TX_DS enabled..
    digitalWrite(CE, 1);
    //delayMicroseconds(20);
    //digitalWrite(CE, 0);
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
void NRF_Send(unsigned char *buf_tx)
{
    SPI_Write_Buf(WR_TX_PLOAD,buf_tx,TX_PLOAD_WIDTH);
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
unsigned char NRF_CheckAck(void)
{
    unsigned char sta = SPI_Read(STATUS);
    if(sta & TX_DS)  //BIT5
    {
        SPI_RW_Reg(FLUSH_TX,0x00);
        SPI_RW_Reg(WRITE_REG+STATUS,0xff);
        return(0);
    }
    if(sta & MAX_RT)
    {        
        SPI_RW_Reg(FLUSH_TX,0);
        SPI_RW_Reg(WRITE_REG+STATUS,0xff);
        return(0);
    }
    else
        return(1);
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Function: RX_Mode();
// 
// Description:
// This function initializes one nRF24L01 device to
// RX Mode, set RX address, writes RX payload width,
// select RF channel, datarate & LNA HCURR.
// After init, CE is toggled high, which means that
// this device is now ready to receive a datapacket.
//----------------------------------------------------------------------------------------------
void NRF_SetRxMode(void)
{
    digitalWrite(CE, 0);
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, TX_ADR_WIDTH);   // Use the same address on the RX device as the TX device
    SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);                               // Enable Auto.Ack:Pipe0 0x01
    SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);                           // Enable Pipe0
    SPI_RW_Reg(WRITE_REG + RF_CH, 40);                                 // Select RF channel 40
    SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);                  // Select same RX payload width as TX Payload width
    SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);                            // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
    SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);                              // Set PWR_UP bit, enable CRC(2 unsigned chars) & Prim:RX. RX_DR enabled..
    digitalWrite(CE, 1);                                               // Set CE pin high to enable RX device
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
unsigned char NRF_Receive(unsigned char *buf_rx)
{
    unsigned char sta = SPI_Read(STATUS);
    //debug use
    //Serial.print("status = ");
    //Serial.println(sta,HEX);
    unsigned char rx_state = sta & RX_DR;  //BIT4
    if(rx_state)
    {
        SPI_Read_Buf(RD_RX_PLOAD, rx_buf, TX_PLOAD_WIDTH);             // read playload to rx_buf         
        SPI_RW_Reg(FLUSH_RX,0);       // clear RX_FIFO  
        SPI_RW_Reg(WRITE_REG+STATUS,sta);                             // clear RX_DR or TX_DS or MAX_RT interrupt flag
        return 1;
    }
    else
        return 0;
}
//----------------------------------------------------------------------------------------------

#endif
