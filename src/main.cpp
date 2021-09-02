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
static CircularBuffer<String, BUFFERSIZE> FWDBuffer;

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
  Serial5.begin(115200);
  Serial5.print("G28\nG1X30Y30Z30\nG28");
}
char* nextFWDMSG[150];
void loop() {  
  static int count = 0;
  if(Serial5.available())
  {
    ReadSerial5();
  }
  else
  {
    if(!FWDBuffer.isEmpty())
    {
      Serial.println("dequeue!");
      String data = FWDBuffer.pop();
      Serial5.print(data);
      Serial5.print("\n");
      Serial.print("SENTDATA:");
      Serial.print(data);
      Serial.print("\n");
    }
  }
  serialReciever->handleSerial();
  motion->tic();

}

void setGalvoPosition(double x, double y)
{
  int tmp_x, tmp_y;
  if(AXIS_INVERSE_X)
    tmp_x = map(x, 0.0,X_MAX_POS_MM, 65535,0)+0.5;
  else
    tmp_x = map(x, 0.0,X_MAX_POS_MM, 0,65535)+0.5;

  if(AXIS_INVERSE_Y)
    tmp_y = map(y, 0.0,Y_MAX_POS_MM, 65535,0)+0.5;
  else
    tmp_y = map(y, 0.0,Y_MAX_POS_MM, 0,65535)+0.5;

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

void setNextFWDMSG(char MSG[150])
{
  
  Serial.print("Adding to main buffer: ");
  String str = String(MSG);
  Serial.println(str);
  FWDBuffer.unshift(str);
  
      Serial.print("Added: ");
      
      Serial.println(str);
}

bool ReadSerial5()
{
  bool retval = false;
  if (Serial5.available()) {
    retval = true;
    static char worda[COMMAND_SIZE], *pSdata=worda;
    byte ch;

    ch = Serial5.read();
    //mcnt++;
    // -1 for null terminator space
    if ((pSdata - worda)>=COMMAND_SIZE-1) {
        pSdata--;
        Serial.print("Serial5: BUFFER OVERRUN\n");
    }

    *pSdata++ = (char)ch;
    if (ch=='\n' || ((pSdata - worda)>=COMMAND_SIZE-3))// Command received and ready.
    {
      
      pSdata = worda;
      Serial.print("\nECHO Serial5: ");Serial.println(worda);
      xinit_process_string(worda);
    }
  }
  return retval;
}

void xinit_process_string(char instruction[])  {
  //init our command
  for (byte i=0; i<COMMAND_SIZE; i++)
    instruction[i] = 0;
  //mcnt = 0;
}