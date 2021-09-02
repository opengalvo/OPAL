/*
  main.h - OPAL FW on PJRC Teensy 4.x board

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

#ifndef MAIN_h
#define MAIN_h

#include <Arduino.h>
//#include <HardwareSerial.h>
#include <CircularBuffer.h>
#include <XY2_100.h>
#include "Pins.h"
#include "Helpers.h"
#include "LaserController.h"
#include "Synrad48Ctrl.h"
#include "MotionMGR.h"
#include "SerialCMDReader.h"

static bool AXIS_INVERSE_X = true;
static bool AXIS_INVERSE_Y = false;

void process();
void setLaserPower(double PWM); 
void setGalvoPosition(double x, double y);
bool ReadSerial5();
void xinit_process_string(char instruction[]);
void setNextFWDMSG(char MSG[150]);



//int mcnt = 0;

#endif