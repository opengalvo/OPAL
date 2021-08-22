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

#pragma once

#ifdef __AVR__
#error "Sorry, this only works on 32 bit Teensy boards.  AVR isn't supported."
#endif

#if TEENSYDUINO < 121
#error "Minimum PJRC Teensyduino version 1.21 is required"
#endif

#ifndef SYNRAD48CTRL_h
#define SYNRAD48CTRL_h

#include <Arduino.h>
#include "pins.h"

class Synrad48Ctrl {
	public:
		Synrad48Ctrl();
   
    void begin(uint16_t,uint16_t);
    void stop(void);
    bool isInitiallized();
		void handleLaser();
		void setLaserPWM(uint16_t PWM);
   


	private:
    int                     laserState;
    //uint16_t                laserEnable_Pin;  // = 0;
    uint16_t                laserPWM_OUT_Pin; // = LASER_OUT_PIN;
    uint16_t                laserPSU_SSR_Pin; // = 10;
    
    bool                    laser_Shutter       = false;
    const static uint16_t   ticklePWM           = 4;
    int                     laserPWM            = 0;
    int                     laserPWMLowerLimit  = 50;
    uint32_t                tickleStart         = 0x0;
    uint32_t                laserInitTime       = {0x1388}; //Millis to wait in warmup state / Tickle state during init
    int                     currentFreq         =0;

void set20kPWM(int PWM) {
  if(currentFreq != 20000) {
    currentFreq = 20000;
    //Serial.print("\n Set 20");
    analogWriteFrequency(LASER_PWM_OUT_PIN, 20000);
  }
  //Serial.print("\n 20k PWM: Setting pin");Serial.print(LASER_PWM_OUT_PIN);Serial.print(" to: ");Serial.print(laserPWM);
  analogWrite(LASER_PWM_OUT_PIN, laserPWM); //Output Laser
  
}

void set5kPWM() {
  if(currentFreq != 5000) {
    currentFreq = 5000;
    //Serial.print("\n Set 5");
    analogWriteFrequency(LASER_PWM_OUT_PIN, 5000);
  }
  
  //Serial.print("\n 5k PWM: Setting pin ");Serial.print(LASER_PWM_OUT_PIN);Serial.print(" to: ");Serial.print(Synrad48Ctrl::ticklePWM);
  analogWrite(LASER_PWM_OUT_PIN, Synrad48Ctrl::ticklePWM); //Output Trickle
}
};
#endif
