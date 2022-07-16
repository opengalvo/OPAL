/*
  Helpers.cpp - Helper functions to be used by OPAL FW on PJRC Teensy 4.x board

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

uint64_t nanos()  // Code by luni @ https://forum.pjrc.com/threads/60493-A-Higher-Resolution-Micros
{
    static uint32_t oldCycles = ARM_DWT_CYCCNT;
    static uint64_t highValue = 0;

    uint32_t newCycles = ARM_DWT_CYCCNT;
    if (newCycles < oldCycles)
    {
        highValue += 0x0000'0001'0000'0000;
    }
    oldCycles = newCycles;
    return (highValue | newCycles) * (1E9/F_CPU);
}

void printWelcome()
{
  Serial.print("\n\n\n");
  Serial.println("********************************************");
  Serial.println("* Teensy running OpenGalvo OPAL Firmware   *");
  Serial.println("*  -This is BETA Software                  *");
  Serial.println("*  -Do not leave any hardware unattended!  *");
  Serial.println("********************************************");
}
