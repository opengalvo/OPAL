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


SerialCMDReader::SerialCMDReader(CircularBuffer<GCode, BUFFERSIZE> *buf)
{
  bufRef = buf;
}

void SerialCMDReader::begin(){
  
  }

void SerialCMDReader::stop(){}

void SerialCMDReader::handleSerial()
{
  if(!SerialCMDReader::bufRef->isFull())
    if (Serial.available()) {

      static char worda[COMMAND_SIZE], *pSdata=worda;
      byte ch;

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
          SerialCMDReader::bufRef->unshift(*tmp);
          delete tmp;
        }
        init_process_string(worda);
      }
   }
}

GCode* SerialCMDReader::process_string(char instruction[])
{
  //process our command!
  
  GCode* newCode = new GCode();
  //TODO: determine if delete newcode is needed to keep memmory clean...
  
  if(has_command('M', instruction, cnt))        {
    newCode->codeprefix = 'M';
    newCode->code = (double)search_string('M', instruction, cnt);

    newCode->s = getVal('S', instruction, cnt);
    
    Serial.println("ok");  
    return newCode;
  } // END of Mcode
  if(has_command('G', instruction, cnt))        {
    newCode->codeprefix = 'G';
    newCode->code = (double)search_string('G', instruction, cnt);

    newCode->x = getVal('X', instruction, cnt);
    newCode->y = getVal('Y', instruction, cnt);
    newCode->z = getVal('Z', instruction, cnt);
    newCode->e = getVal('E', instruction, cnt);
    newCode->a = getVal('A', instruction, cnt);
    newCode->b = getVal('B', instruction, cnt);
    newCode->c = getVal('C', instruction, cnt);
    newCode->f = getVal('F', instruction, cnt);
    newCode->i = getVal('I', instruction, cnt);
    newCode->j = getVal('J', instruction, cnt);
    newCode->p = getVal('P', instruction, cnt);
    newCode->r = getVal('R', instruction, cnt);
    newCode->s = getVal('S', instruction, cnt);
    newCode->t = getVal('T', instruction, cnt);

    Serial.println("ok");  
    return newCode;
  } //END of Gcode
  else 
  {
    newCode->x = getVal('X', instruction, cnt);
    newCode->y = getVal('Y', instruction, cnt);
    newCode->z = getVal('Z', instruction, cnt);
    newCode->e = getVal('E', instruction, cnt);
    newCode->a = getVal('A', instruction, cnt);
    newCode->b = getVal('B', instruction, cnt);
    newCode->c = getVal('C', instruction, cnt);
    newCode->f = getVal('F', instruction, cnt);
    newCode->i = getVal('I', instruction, cnt);
    newCode->j = getVal('J', instruction, cnt);
    newCode->p = getVal('P', instruction, cnt);
    newCode->r = getVal('R', instruction, cnt);
    newCode->s = getVal('S', instruction, cnt);
    newCode->t = getVal('T', instruction, cnt);

    Serial.println("ok");  
    return newCode;
  }
}
