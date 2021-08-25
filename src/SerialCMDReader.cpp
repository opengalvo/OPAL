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
#include <CircularBuffer.h>
#include "helpers.h"
#include "SerialCMDReader.h"


SerialCMDReader::SerialCMDReader()
{
}

void SerialCMDReader::begin(CircularBuffer<GCode, BUFFERSIZE> *buf){
  commandBuffer = buf;
  }

void SerialCMDReader::stop(){}

void SerialCMDReader::handleSerial()
{
  static char worda[COMMAND_SIZE], *pSdata=worda;
  byte ch;
  
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

        /* the character / means delete block... used for comments and stuff.*/
        if (worda[0] == '/' || worda[0] == '(' || worda[0] == ';')
        {
          Serial.println("ok");
          return;
        }
        else
        {
          GCode* tmp = SerialCMDReader::process_string(worda);
          SerialCMDReader::commandBuffer->unshift(*tmp);
          delete tmp;
        }
        init_process_string(worda);
      }
   }
}

GCode* SerialCMDReader::process_string(char instruction[])
{
  //process our command
  GCode* newCode = new GCode();
  
  if(SerialCMDReader::has_command('M', instruction, cnt))        {
    newCode->codeprefix = 'M';
    newCode->code = (double)search_string('M', instruction, cnt);
  }
  else if(has_command('G', instruction, cnt))        {
    newCode->codeprefix = 'G';
    newCode->code = (double)search_string('G', instruction, cnt);
  }
  else
  {
    newCode->codeprefix = 'G';
  }
  char parameters[] = {'X','Y','Z','E','A','B','C','F','I','J','P','R','S','T'};
  for(int i = 0; i < 14; i++)
  {
    if(has_command(parameters[i], instruction, cnt))
      newCode->x = ((double)search_string(parameters[i], instruction, cnt));
    else
      newCode->x = MAX_VAL;
  }
  Serial.println("ok");  
  return newCode;
}
