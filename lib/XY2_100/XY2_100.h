/*
  XY2_100.h - driver code for XY2-100 protocol on PJRC Teensy 4.x board

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
#error "Error, this only works on Teensy 4.0 boards.  AVR is not supported."
#endif

#if TEENSYDUINO < 121
#error "Teensyduino version 1.21 or later is required to compile this library."
#endif

#ifndef XY2_100_h
#define XY2_100_h


#include <Arduino.h>



class XY2_100 {
public:
  XY2_100();
  void begin(void);
  void goTo(uint16_t x, uint16_t y);
  static uint8_t parity(uint16_t v);
  
private:
  static void QT1_isr();
  unsigned long buildMsg(uint16_t data);

  const static int pwm_pin_clock = 22;
  const static int pwm_pin_sync = 17;  
  const static int pwm_pin_dataX = 19;
  const static int pwm_pin_dataY = 14;
};

#endif
