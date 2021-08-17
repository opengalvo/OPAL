/*
  Helpers.h - Helper functions to be used by OPAL FW on PJRC Teensy 4.x board

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

#ifndef HELPERS_H
/*
 * Projection size 250mm x 250mm
 * Number of positions per mm = 65536/projection size mm
 *                            = 262.144
 */

#define MM_TO_POSITION_RATIO 262.144 //  --> 250mmx250mm  ==> G1X3 (move X: 3mm) = 3 * 65536/250 (num pos per mm)

#define MAX_VAL 2147483630

  uint64_t nanos();
struct coordinate {
  int x = MAX_VAL;
  int y = MAX_VAL;
  int z = MAX_VAL;
};
  
  struct GCode {

  char codeprefix;
  int code; // Go = 0 , G28 = 28 etc. Only G0 / G1 Supported so far...
  int x;
  int y;
  int z;
  int e;

  int i;
  int j;

  int p;

  int s; // Laser Power
  int f; // FeedRate
  int r; // Misc
  int t; // Misc
  double moveLengthNanos;
};


void printDouble( double val, unsigned int precision);
  #define HELPERS_H
#endif
