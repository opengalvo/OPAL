/*
  OpenGalvo.ino - Main projectfile to run OPAL FW on PJRC Teensy 4.x board

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


#define LASER_IS_SYNRAD

#include <Arduino.h>
#include <HardwareSerial.h>
#include "Pins.h"
#include "Helpers.h"

#ifdef LASER_IS_SYNRAD
//#include "Synrad48Ctrl\Synrad48Ctrl.h"
#include "Synrad48Ctrl.h"
Synrad48Ctrl laser;
#define LASER_MAX 4096
#endif


#include "XY2_100.h"
XY2_100 galvo;

#include <CircularBuffer.h>
CircularBuffer<GCode, 50> commandBuffer;
CircularBuffer<GCode, 50> mBuffer;

#include "SerialCMDReader.h"
SerialCMDReader serialReciever;

uint64_t next_command_time = 0;

bool beginNext = true;
static uint64_t startNanos;
static uint64_t endNanos;

static uint64_t _now;

static coordinate lastMove;
coordinate to;

static int distx;
static int disty;
static int distz;

static int feedrate;

GCode * oldPreviousMove;
GCode * previousMove;
GCode * currentMove;
GCode * nextMove;

void setup() {
  lastMove.x = 0;
  lastMove.y = 0;
  lastMove.z = 0;

  to.x = 0;
  to.y = 0;
  to.z = 0;
  
  Serial.begin(115200);
#ifdef LASER_IS_SYNRAD
  laser.begin(LASER_PWM_OUT_PIN, LASER_OUT_PIN);
#endif
  galvo.begin(); //TODO:ADD define "Galvo has SSR" for galvo PSU
  serialReciever.begin(&commandBuffer);
}

coordinate target;
  
void loop() {  
  //delay(100);
  serialReciever.handleSerial();
  process();
#ifdef LASER_IS_SYNRAD
  laser.handleLaser();
#endif
}


void process()  {
  _now = nanos();
  
  if(beginNext)  {
    
    //Serial.print("\n Before processing Mcodes"); 
    while(!mBuffer.isEmpty())    {
      //processMCodes
      GCode* cmd = new GCode(mBuffer.shift());
        processMcode(cmd);
        delete cmd;
    }
    //Serial.print("\n - After processing Mcodes"); 
    
    delete oldPreviousMove; 
    oldPreviousMove = previousMove;
    previousMove = currentMove;
    currentMove = nextMove;

    bool gcodeFound = false;
    while(!gcodeFound && !commandBuffer.isEmpty())
    {
      nextMove = new GCode((commandBuffer.shift()));
      if((*nextMove).codeprefix != 'G'){
        //Serial.print("\n - Non GCode moved to next buffer"); 
        mBuffer.unshift(*nextMove);
      }
      else{
        gcodeFound = true;  
      }       
    }      
    if(!gcodeFound){
      nextMove = NULL;
    }

    // Buffer MGMT is done 

    //Allways update history...
    if(previousMove)  //SET UP PREVIOUS POSITION
    {
      if((*previousMove).x != MAX_VAL)
        lastMove.x = *(new int((*previousMove).x));
      if((*previousMove).y != MAX_VAL)
        lastMove.y = *(new int((*previousMove).y));
      if((*previousMove).z != MAX_VAL)
        lastMove.z = *(new int((*previousMove).z));
    }
    
    if(currentMove)  { 

      if((*currentMove).x != MAX_VAL)
        to.x = (*currentMove).x;
      if((*currentMove).y != MAX_VAL)
        to.y = (*currentMove).y;
      if((*currentMove).z != MAX_VAL)
        to.z = (*currentMove).z;
      if((*currentMove).f != MAX_VAL)
        feedrate = (*currentMove).f;

      startNanos = _now;      
      distx = to.x-lastMove.x;
      disty = to.y-lastMove.y;
      distz = to.z-lastMove.z; // not used.....
      calculateMoveLengthNanos(distx, disty, feedrate, &((*currentMove).moveLengthNanos));
      endNanos = startNanos + (*currentMove).moveLengthNanos;

      beginNext = false;
    }
  }

  //interpolate  
  if(_now > endNanos)  //We are out of time!
  {
    //Serial.print("\n End Of Move: GALVO - Go to x:");Serial.print(to.x);Serial.print(" Y:");Serial.print(to.y);
    galvo.goTo(to.x, to.y); //Make sure to hit the commanded position
    beginNext = true;
    return;
  }
  else
  {


    distx = to.x-lastMove.x;
    disty = to.y-lastMove.y;
    distz = to.z-lastMove.z; // not used.....
    //Serial.print("\n Interpolate: DBG - lastMove X:");Serial.print(lastMove.x);Serial.print(" to.X::");Serial.print(to.x);
    uint64_t t = (*currentMove).moveLengthNanos; 
    double fraction_of_move = (double)(_now-startNanos)/t;
    int x = (lastMove.x + (distx*fraction_of_move));
    int y = (lastMove.y + (disty*fraction_of_move));
    //Serial.print("\n Interpolate: GALVO - Go to x:");Serial.print(x);Serial.print(" / ");Serial.print(to.x);Serial.print(" Y:");Serial.print(y);Serial.print(" / ");Serial.print(to.y);
    galvo.goTo( x,y );
    return ;
  }
  return;
}

void calculateMoveLengthNanos(int xdist, int ydist, int moveVelocity, double* result)  {  //Velocity is presumed to be in coordinates/s

  double lengthOfMove = sqrt( (0.0 + xdist)*(0.0 + xdist)  + (0.0 + ydist)*(0.0 + ydist) ); // calc hypo a^2+b^2=c^2
  //TODO: Verify unit conversion
  //Serial.print("\nCALC:movelength: ");Serial.print(xresult);
  *result = ((lengthOfMove*1000*1000*1000/moveVelocity)); //s=v*t -> s/v =t   (movelength/moveVolocity) -> 2pos / (4pos/s) = 0.5s *1000 = 500ms
  return;
}

void processMcode(GCode* code)
{
  //Serial.print("\nExecuting MCode: ");Serial.print(code->codeprefix);Serial.println(code->code);
  //Serial.print("-- Executing MCode: Not Implemented\n");
}
