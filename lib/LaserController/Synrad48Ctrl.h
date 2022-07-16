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
#include "LaserController.h"

class Synrad48Ctrl : public LaserController {
	public:
		Synrad48Ctrl();
    void begin(int PWM_OUT_Pin, int PSU_SSR_Pin);
    void stop(void);
    bool isInitiallized();
    bool isHalted();
		void handleLaser();
		void update(uint16_t PWM);
		void update();
   
	private:
    int                     laserState;
    uint16_t                laserPWM_OUT_Pin; // = LASER_OUT_PIN;
    uint16_t                laserPSU_SSR_Pin; // = 10;
    
    bool                    laser_Shutter       = false;
    const static uint16_t   ticklePWM           = 21;
    int                     laserPWM            = 0;
    int                     laserPWMLowerLimit  = 50;
    uint32_t                tickleStart         = 0x0;
    uint32_t                laserInitTime       = {0x1388}; //Millis to wait in warmup state / Tickle state during init
    int                     currentFreq         = 0;
    int                     oldlaserPWM         = 0;
    bool                    _isHalted            = true;

void set20kPWM(int PWM) {
  if(currentFreq != 20000) {
    currentFreq = 20000;
    analogWriteFrequency(laserPWM_OUT_Pin, 20000);
  }
  analogWrite(laserPWM_OUT_Pin, laserPWM); //Output Laser
}

void set5kPWM() {
  if(currentFreq != 5000) {
    currentFreq = 5000;
    analogWriteFrequency(laserPWM_OUT_Pin, 5000);
  }
  analogWrite(laserPWM_OUT_Pin, Synrad48Ctrl::ticklePWM); //Output Trickle
}

};
#endif
