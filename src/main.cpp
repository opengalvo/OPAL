/*
  main.cpp - Main projectfile to run OPAL FW on PJRC Teensy 4.x board

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

#include "main.h"


static CircularBuffer<GCode, BUFFERSIZE> commandBuffer;

MotionMGR* motion;

XY2_100* galvo;

SerialCMDReader *serialReciever;

#ifdef LASER_IS_SYNRAD
//Synrad48Ctrl syn;
#endif
LaserController *laser;
void setup() {
  serialReciever = new SerialCMDReader(&commandBuffer);
  serialReciever->begin();

  #ifdef LASER_IS_SYNRAD
  laser = new Synrad48Ctrl();
  #else
  //implement PWMLaser
  #endif
  laser->begin(LASER_PWM_OUT_PIN, LASER_SSR_OUT_PIN);
  //init Galvo Protocol
  galvo = new XY2_100();
  galvo->begin(); //TODO:ADD define "Galvo has SSR" for galvo PSU

  motion = new MotionMGR(&commandBuffer);
  motion->begin(galvo, laser);

}

void loop() {  
  serialReciever->handleSerial();
  motion->tic();
}

void setGalvoPosition(double x, double y)
{
  int tmp_x = map(x, 0.0,X_MAX_POS_MM, 65535,0)+0.5;
  int tmp_y = map(y, 0.0,Y_MAX_POS_MM, 65535,0)+0.5;

  galvo->setPos(tmp_x, tmp_y);
}

void setLaserPower(double PWM)
{
  double tmp_PWMMin = LASER_MIN_PWM_PERCENT;
  double tmp_Max = LASER_MAX;
  int tmp_LaserRes = LASER_RESOLUTION;
  double pinVal = map(PWM,0.0,tmp_Max,tmp_PWMMin,(exp2(tmp_LaserRes))+0.0);
  laser->update((int)pinVal);
}


