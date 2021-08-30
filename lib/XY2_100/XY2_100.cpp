/*
  XY2_100.cpp - driver code for XY2-100 protocol on PJRC Teensy 4.x board

  Part of OpenGalvo - OPAL Firmware

  Copyright (c) 2020-2021 Daniel Olsson

  OPAL Firmware is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPAL Firmware is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPAL Firmware.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "XY2_100.h"

volatile bool anewPacketX[20] = { false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
volatile bool anewPacketY[20] = { false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
volatile bool bnewPacketX[20] = { false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
volatile bool bnewPacketY[20] = { false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };

volatile bool *activePacketX;  
volatile bool *activePacketY; 

XY2_100::XY2_100()
{

}

void XY2_100::begin()
{
    //ISR
  pinMode(0, OUTPUT);                   //pin 0 as digital output (ISR timing)

  // Data //
  pinMode(pwm_pin_clock, OUTPUT);
  pinMode(pwm_pin_sync, OUTPUT);
  pinMode(pwm_pin_dataX, OUTPUT);
  pinMode(pwm_pin_dataY, OUTPUT);

  //turn on clocks for Timer QT1 (CG13)
  CCM_CCGR6 |= CCM_CCGR6_QTIMER1(CCM_CCGR_ON);

  //TMR1 Setup =========================
  //Disable all timers for QT1
  TMR1_ENBL = 0;
  TMR1_CTRL1 = 0;                       // stop/intialise  all functions of QT1 Timer1

  //status and control
  TMR1_SCTRL1 = 0;                      // clear before set
  TMR1_SCTRL1 |= 0b0000000000000001;    //no compare interrupt, no overflow, input edge flag enabled, rising capture, no forcing, no invert, external pin is OFLAG  

  //Counter
  TMR1_LOAD1 = 0;                       // counter starts from zero
  TMR1_COMP11 = 37-1;                   // count up to this value (Calibrated to 2MHz at cl+ Output (37))
  TMR1_CMPLD11 = 37-1;                  // load compare register with value from this register

  //comparator status and control
  TMR1_CSCTRL1 = 0;                     // clear before set
  TMR1_CSCTRL1 |= 0b0000001001000001;   // no debug, no fault, no altload, no reload, no trigger, count up, TCF1EN, no COMP2, COMP1
 
  //configure Teensy pin Capture pin
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 1; // sets up the "external pin" for QT1 Timer1 (see R.M. page 309).

  //start QT1 Timer1
  TMR1_CTRL1 = 0b0011000000100011;      // 001(Count rising edges Primary Source),1000(IP Bus Clock),00 (Secondary Source = 0),
  TMR1_ENBL = 0b0000000000000010;       // enable Timer1
 
  //prepare interrupt parameters
  attachInterruptVector(IRQ_QTIMER1, XY2_100::QT1_isr);
  NVIC_ENABLE_IRQ(IRQ_QTIMER1);

  activePacketX = anewPacketX;
  activePacketY = anewPacketY;  
}

short bitCount = 0;
volatile bool hasData = 1;              // New XY2-100 Packet is available
volatile bool selector = true;          // Switches between the two packet arrays
volatile bool cbitx = 0;                // The current databit for the x wire
volatile bool cbity = 0;                // The current databit for the y wire


bool ClkHiLo = true;                    // Next clock state
uint32_t d = 0UL;
uint32_t nextbit = 0UL;

void XY2_100::QT1_isr() {  
  //ISR for QT1
  TMR1_CSCTRL1 &= ~(TMR_CSCTRL_TCF1);
  asm volatile ("dsb");                   // wait for clear memory barrier
  GPIO6_DR = (GPIO6_DR & ~(GPIO6_DR)) | (d);

  //TMR1_CSCTRL1 &= ~(TMR_CSCTRL_TCF1);         // clear comp flag
  if(ClkHiLo)
  {
    d = 0UL;

        //WRITE SIGNAL
    cbitx = activePacketX[bitCount];
    cbity = activePacketY[bitCount];
    d = (d & ~(1UL << 18)) | (cbity << 18); //PORT D3 --> pin14
    d = (d & ~(1UL << 16)) | (cbitx << 16);  //PORT D2 --> pin19
   
    //WRITE SYNC
    if(bitCount >= 19)
    {
      bitCount = -1;
      d = (d & ~(1UL << 22)) | (0 << 22);   // PORT D1 --> pin 17
      //d = (d & ~(1UL << 23)) | (1 << 23);   // PORT D4 --> pin 16

      //If we have new data, point the activePacket pointer to the array containing the new data
      if(hasData == 1)
      {
        if(selector)
        {
          activePacketX = anewPacketX;
          activePacketY = anewPacketY;
        }
        else
        {
          activePacketX = bnewPacketX;
          activePacketY = bnewPacketY;
        }
        selector = !selector;
      }
      hasData = 0;
    }
    else
    {
      d = (d & ~(1UL << 22)) | (1 << 22);     // PORT D1 --> pin 17
      //d = (d & ~(1UL << 23)) | (0 << 23);   // PORT D4 --> pin 16
    }
   
    //WriteClkHi
    d = (d & ~(1UL << 24)) | (1 << 24);       // PORT D1 --> pin 22
    //d = (d & ~(1UL << 25)) | (0 << 25);     // PORT D1 --> pin 23
    bitCount++;
  }
  else
  {
    //Write ClkLow
    d = (d & ~(1UL << 24)) | (0 << 24);       // PORT D1 --> pin 22
    //d = (d & ~(1UL << 25)) | (1 << 25);     // PORT D1 --> pin 23
  }

  ClkHiLo = !ClkHiLo;
}

unsigned long XY2_100::buildMsg(uint16_t data)
{
  unsigned long msg = 0;
  msg = msg | 0;
  msg = msg << 1;  msg = msg | 0;
  msg = msg << 1;  msg = msg | 1;
  msg = msg << 16; msg = msg | data;
  msg = msg << 1; msg = msg | XY2_100::parity(data);
  //msg = msg << 1; msg = msg | 1;
  return msg;
}

uint8_t XY2_100::parity(uint16_t v)
{
    uint8_t t = (uint8_t) v ^ v >> 8;
    t ^= t >> 4;
    t ^= t >> 2;
    t ^= t >> 1;
    return (t & 1);
}

void XY2_100::setPos(uint16_t x,uint16_t y)
{
  
    unsigned long dataX = XY2_100::buildMsg(x);
    unsigned long dataY = XY2_100::buildMsg(y);
    if(selector)
      for(int i = 0; i < 20; i++)
      {      
        bnewPacketX[i] = bitRead(dataX, 19- i);
        bnewPacketY[i] = bitRead(dataY, 19 - i);      
      }
    else
      for(int i = 0; i < 20; i++)
      {      
        anewPacketX[i] = bitRead(dataX, 19- i);
        anewPacketY[i] = bitRead(dataY, 19 - i);          
      }
    hasData = 1;
}
