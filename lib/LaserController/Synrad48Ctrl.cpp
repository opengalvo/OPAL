/*
  Synrad48Ctrl.h - driver code for Synrad 48 Series laser on PJRC Teensy 4.x board

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

#include <Arduino.h>

//#ifndef TEENSY_TIMER_TOOL
//#include <TeensyTimerTool.h>
//using namespace TeensyTimerTool;
//#define TEENSY_TIMER_TOOL
//#endif

#include <string.h>
#include "Synrad48Ctrl.h"
#define LASER_RESOLUTION 12


//PeriodicTimer t2;

Synrad48Ctrl::Synrad48Ctrl()
{
  laserState = 30;
}

uint16_t LastPWMRequest = 0;

void Synrad48Ctrl::update(uint16_t PWM)
{
  laserPWM = PWM;
  handleLaser();
  }
void Synrad48Ctrl::update()
{
  handleLaser();
}

void Synrad48Ctrl::begin(int PWM_OUT_Pin, int PSU_SSR_Pin)
{
  if(_isHalted)
  {
    if(laserState != 30)
    {
      // ERROR:
      Serial.write("\nLaser begin error: Laser in unexpected state\n");
    }
    else
    {
      laserPWM_OUT_Pin = PWM_OUT_Pin;
      laserPSU_SSR_Pin = PSU_SSR_Pin;
      analogWriteResolution(LASER_RESOLUTION);
      pinMode(laserPWM_OUT_Pin, OUTPUT);
      pinMode(laserPSU_SSR_Pin, OUTPUT);
      
      digitalWrite(laserPSU_SSR_Pin,0);
      analogWrite(laserPWM_OUT_Pin,0);
      delay(laserInitTime + 10 );
      digitalWrite(laserPSU_SSR_Pin,1);
      // waist of recources - call handle laser at the beginning of each command instead. //t2.begin(this->handleLaser, 50); //50us = 20kHz
    
      laserState = 0;
      laserPWM = 0;
      oldlaserPWM = 0;
      _isHalted = false;
    }

  }
}

void Synrad48Ctrl::stop()
{
  laserPWM = 0;
  digitalWrite(laserPSU_SSR_Pin,0);
  analogWrite(laserPWM_OUT_Pin,0);
  laserState = 30;
  _isHalted = true;
}

bool Synrad48Ctrl::isHalted()
{
  return _isHalted;
}

bool Synrad48Ctrl::isInitiallized()
{
  if(laserState==3 || laserState==4)
    return true;
   else
    return false;
}

void Synrad48Ctrl::handleLaser()
{
  if(isInitiallized())
  {
    #ifdef PIN13_LED_INDICATES_LASER_READY
    digitalWrite(13,1);
    #endif
    if( laserPWM==oldlaserPWM) //Nothing changed
      return;
    if(laserPWM!=oldlaserPWM)
      oldlaserPWM = laserPWM;
  }
  #ifdef PIN13_LED_INDICATES_LASER_READY
  else digitalWrite(13,0);
  #endif
/*   |
 *  LaserStates:
 *       0 = BEGINWARMUP      - LaserEnable_Pin is HIGH, start the timer and go to warmup
 *       1 = WARMUP           - LaserEnable_Pin is HIGH, state is held for 5 seconds
 *       2 = INIT READY       - When PWM commands Lazing to be less than tickle*2 PWM reconfigures to 1us pulse every 200us (5kHz).
 *       3 = READY            - When PWM commands Lazing to be less than tickle*2 PWM reconfigures to 1us pulse every 200us (5kHz).
 *       4 = LAZING           - When PWM commands Lazing to be more than tickle*2 PWM reconfigures to 20kHz and selected pulse width.
 *       30 = DISABLED        - LaserEnable_Pin is HIGH, state is held for 5 seconds
*/

  int laserEnablePinState = HIGH; //TODO: Impl... digitalRead(Synrad48Ctrl::laserEnable_Pin);
  if(laserEnablePinState == LOW)
  {
    laserState = 30;  //TODO: Fix how to get out of this
    tickleStart = 0x0;
    analogWrite(Synrad48Ctrl::laserPWM_OUT_Pin, 0);
  }
  else
  { 
    uint32_t NOW = millis();

    switch (laserState)
    {
      case 0:
      {
        //IN BEGIN WARMUP STATE
        set5kPWM();
        tickleStart = NOW;
        laserState = 1;
        //fallthrough intended
      }
      case 1:
      {
        // IN WARMUP STATE
        bool wait = tickleStart > (NOW-laserInitTime);
        if (!wait)
        {
          //Trickle time has passed
          tickleStart = 0;
          laserState = 2;
        }
        break;
      }
      case 2:
      {
        // IN INIT READY STATE
        this->set5kPWM();
        laserState = 3;
        //fallthrough intended
      }
      case 3:
      {
        // IN READY STATE
        if(laserPWM>laserPWMLowerLimit)
          laserState = 4;
        else
          break;
        //fallthrough intended
      }
      case 4:
      {
        // IN LAZING STATE
        if(laserPWM<=laserPWMLowerLimit)
        {
          //laserState = 2;
          this->set5kPWM();
          laserState = 3;
        }
        else
          //if(!(Synrad48Ctrl::laser_Shutter))
          //TODO: FIX IMPLEMENTATION - Sync with LASER_ENABLED and add shutter to LaserController interface
            this->set20kPWM(laserPWM);
          //else
            //this->set5kPWM();
        break;
      }
      default:
      {
        //laserState = 30;
        //Laser has been disabled
        analogWrite(Synrad48Ctrl::laserPWM_OUT_Pin, 0);
        break;
      }
    }
    
  }
}

