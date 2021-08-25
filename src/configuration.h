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
#ifndef CONFIGURATION_H

#define CMDBUFFERSIZE 50 //Number of cashed GCodes
#define MBUFFERSIZE 20  //Buffersize for MCODES - number of consecutive M-Codes before another G-Code
#define DEFAULT_FEEDRATE 100
#define LASER_IS_SYNRAD
#define LASER_MAX 4096
#define LASER_G0_OFF_G1_ON

#define CONFIGURATION_H
#endif