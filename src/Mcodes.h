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

#ifndef MCODES_H
  #define MCODES_H
  //M17 - Turn all steppers ON -> Galvo PSU Control (SSR)
  //M18 - Turn all steppers OFF -> Galvo PSU Control (SSR)
  //M80 - Laser PSU Control (SSR) Implicit delay for SynradCtrl::laserInitTime milliseconds (5000)
  //M81 - Laser PSU Control (SSR)
#endif
