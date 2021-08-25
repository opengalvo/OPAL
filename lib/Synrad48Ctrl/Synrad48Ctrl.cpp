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

#include <string.h>
#include "Synrad48Ctrl.h"
#define LASER_RESOLUTION 12

Synrad48Ctrl::Synrad48Ctrl()
{
  laserState = 30;
}

uint16_t LastPWMRequest = 0;

void Synrad48Ctrl::setLaserPWM(uint16_t PWM)
{
  laserPWM = PWM;
  handleLaser();
  }

void Synrad48Ctrl::begin(uint16_t PWM_OUT_Pin, uint16_t PSU_SSR_Pin)
{
  if(laserState != 30)
  {
    // ERROR: Serial.writeln("Laser begin error: Laser in unexpected state");
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
    delay(laserInitTime );
    digitalWrite(laserPSU_SSR_Pin,1);
    
    laserState = 0;
    laserPWM = 0;
  }
}

void Synrad48Ctrl::stop()
{
  laserPWM = 0;
  digitalWrite(laserPSU_SSR_Pin,0);
  analogWrite(laserPWM_OUT_Pin,0);
  laserState = 30;
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
    if( laserPWM==oldlaserPWM)
    {
      //Serial.print("\n"); Serial.print(laserPWM); Serial.print(""); Serial.print("Old and new are same"); 
      return;
    }
    if(laserPWM!=oldlaserPWM)
    {
      //Serial.print("\n"); Serial.print(laserPWM); Serial.print("Old and new are Different ======\n========\n========\n======\n======");
      oldlaserPWM = laserPWM;
    }
  }
  
/*   |
 *  LaserStates:
 *       0 = BEGINWARMUP      - LaserEnable_Pin is HIGH, start the timer and go to warmup
 *       1 = WARMUP           - LaserEnable_Pin is HIGH, state is held for 5 seconds
 *       2 = INIT READY       - When PWM commands Lazing to be less than tickle*2 PWM reconfigures to 1us pulse every 200us (5kHz).
 *       3 = READY            - When PWM commands Lazing to be less than tickle*2 PWM reconfigures to 1us pulse every 200us (5kHz).
 *       4 = LAZING           - When PWM commands Lazing to be more than tickle*2 PWM reconfigures to 20kHz and selected pulse width.
 *       30 = DISABLED        - LaserEnable_Pin is HIGH, state is held for 5 seconds
*/

  //Serial.print("\nhandle Laser in state: ");Serial.print(laserState);
  int laserEnablePinState = HIGH; //digitalRead(Synrad48Ctrl::laserEnable_Pin);
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
        digitalWrite(13,1); //Set LED ON
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
          if(!(Synrad48Ctrl::laser_Shutter))
            this->set20kPWM(laserPWM);
          else
            this->set5kPWM();
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

