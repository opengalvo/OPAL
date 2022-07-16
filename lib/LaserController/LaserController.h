/*
  LaserController.h - driver code for Synrad 48 Series laser on PJRC Teensy 4.x board

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

#ifndef LASERCONTROLLER_h
#define LASERCONTROLLER_h

#include <Arduino.h>

class LaserController   
{
    public:
      void virtual begin(int PWM_OUT_Pin, int PSU_SSR_Pin) = 0;
      void virtual stop() = 0;
      void virtual update(uint16_t pwm) = 0;
      void virtual update() = 0;
      bool virtual isHalted();
};

#endif