/*
  SerialCMDReader.cpp - driver code to interpret GCode over Serial on PJRC Teensy 4.x board

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
#include "helpers.h"
#include <CircularBuffer.h>
//#include "Synrad48Ctrl.h"


#include "SerialCMDReader.h"

//
//#ifndef TEENSY_TIMER_TOOL
//#include <TeensyTimerTool.h>
//using namespace TeensyTimerTool;
//#define TEENSY_TIMER_TOOL
//#endif


SerialCMDReader::SerialCMDReader()
{
}

void SerialCMDReader::begin(CircularBuffer<GCode, 50> *buf){
  commandBuffer = buf;
  }

void SerialCMDReader::stop(){}

void SerialCMDReader::handleSerial()
{
  static char worda[COMMAND_SIZE], *pSdata=worda;
  byte ch;
  int val;
  if(!SerialCMDReader::commandBuffer->isFull())
    if (Serial.available()) {
      ch = Serial.read();
      cnt++;
      // -1 for null terminator space
      if ((pSdata - worda)>=COMMAND_SIZE-1) {
         pSdata--;
         Serial.print("BUFFER OVERRUN\n");
      }

      *pSdata++ = (char)ch;

      if (ch=='\n')// Command received and ready.
      {
        
        pSdata = worda;
        SerialCMDReader::commandBuffer->unshift(*SerialCMDReader::process_string(worda));
        init_process_string(worda);
      }
   }
}

GCode* SerialCMDReader::process_string(char instruction[])
{
  //process our command!
    
  //the character / means delete block... used for comments and stuff.
  if (instruction[0] == '/' || instruction[0] == '(')
  {
    Serial.println("ok");
    return;
  }
  
  GCode* newCode = new GCode();
  //TODO: determine if delete newcode is needed to keep memmory clean...
  
  if(SerialCMDReader::has_command('M', instruction, cnt))        {
    newCode->codeprefix = 'M';
    newCode->code = (int)search_string('M', instruction, cnt);
  }
  else if(has_command('G', instruction, cnt))        {
    newCode->codeprefix = 'G';
    newCode->code = (int)search_string('G', instruction, cnt);
  }
  else
  {
    newCode->codeprefix = 'G';
    }
    
  if(has_command('X', instruction, cnt))
  {
    newCode->x = ((int)search_string('X', instruction, cnt))* MM_TO_POSITION_RATIO;
  }
  else
    newCode->x = MAX_VAL;
  if(has_command('Y', instruction, cnt))
    newCode->y = ((int)search_string('Y', instruction, cnt))* MM_TO_POSITION_RATIO;
  else
    newCode->y = MAX_VAL;
  if(has_command('Z', instruction, cnt))
    newCode->z = (int)search_string('Z', instruction, cnt);
  else
    newCode->z = MAX_VAL;
  if(has_command('E', instruction, cnt))
    newCode->e = (int)search_string('E', instruction, cnt);
  else
    newCode->e = MAX_VAL;
  if(has_command('F', instruction, cnt))
    newCode->f = (int)search_string('F', instruction, cnt);
  else
    newCode->f = MAX_VAL;
  if(has_command('I', instruction, cnt))
    newCode->i = (int)search_string('I', instruction, cnt);
  else
    newCode->i = MAX_VAL;
  if(has_command('J', instruction, cnt))
    newCode->j = (int)search_string('J', instruction, cnt);
  else
    newCode->j = MAX_VAL;
  if(has_command('P', instruction, cnt))
    newCode->p = (int)search_string('P', instruction, cnt);
  else
    newCode->p = MAX_VAL;
  if(has_command('R', instruction, cnt))
    newCode->r = (int)search_string('R', instruction, cnt);
  else
    newCode->r = MAX_VAL;
  if(has_command('S', instruction, cnt))
    newCode->s = (int)search_string('S', instruction, cnt);
  else
    newCode->s = MAX_VAL;
  if(has_command('T', instruction, cnt))
    newCode->t = (int)search_string('T', instruction, cnt);
  else
    newCode->t = MAX_VAL;

  Serial.println("ok");  
  return newCode;
}
