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

//efine MM_TO_POSITION_RATIO 262.144 //  --> 250mmx250mm  ==> G1X3 (move X: 3mm) = 3 * 65536/250 (num pos per mm)

#define MAX_VAL 2147483630
#define BUFFERSIZE 20
#define DEFAULT_FEEDRATE 100
  uint64_t nanos();
struct coordinate {
  double x = MAX_VAL;
  double y = MAX_VAL;
  double z = MAX_VAL;
};
  
  struct GCode {

  char codeprefix;
  int code; // Go = 0 , G28 = 28 etc. Only G0 / G1 Supported so far...
  double x;
  double y;
  double z;
  double e;

  double i;
  double j;

  double p;

  double s; // Laser Power
  double f; // FeedRate
  double r; // Misc
  double t; // Misc
  double moveLengthNanos;
};


void printDouble( double val, unsigned int precision);
  #define HELPERS_H
#endif
