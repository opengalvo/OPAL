/*
  OPAL.ino - Main projectfile to run OPAL FW on PJRC Teensy 4.x board

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
#include <HardwareSerial.h>
#include "Pins.h"
#include "Helpers.h"
#include "main.h"

#ifdef LASER_IS_SYNRAD

#include <Synrad48Ctrl.h>

Synrad48Ctrl laser;

#endif


#include "XY2_100.h"
XY2_100 galvo;

#include <CircularBuffer.h>
CircularBuffer<GCode, BUFFERSIZE> commandBuffer;
CircularBuffer<GCode, BUFFERSIZE> mBuffer;

#include "SerialCMDReader.h"
SerialCMDReader serialReciever;

uint64_t next_command_time = 0;

bool beginNext = true;
static uint64_t startNanos;
static uint64_t endNanos;

static uint64_t _now;

static coordinate lastMove;
coordinate to;

static double distx;
static double disty;
static double distz;

static double feedrate = 100;//DEFAULT_FEEDRATE;
//double mul = MM_TO_POSITION_RATIO;

GCode * oldPreviousMove;
GCode * previousMove;
GCode * currentMove;
GCode * nextMove;

coordinate target;
int lastLaserPWR = 0;
bool laserChanged = false;
int itcnt = 0;
static int interpolCnt = 0;

void setup() {
  lastMove.x = 0;
  lastMove.y = 0;
  lastMove.z = 0;

  to.x = 0;
  to.y = 0;
  to.z = 0;
  
  Serial.begin(115200);
#ifdef LASER_IS_SYNRAD
  laser.begin(LASER_PWM_OUT_PIN, LASER_SSR_OUT_PIN);
#endif
  galvo.begin(); //TODO:ADD define "Galvo has SSR" for galvo PSU
  serialReciever.begin(&commandBuffer);
}

void calculateMoveLengthNanos(double xdist, double ydist, double moveVelocity, double* result)  {  //Velocity is presumed to be in coordinates/s

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
        lastMove.x = *(new double((*previousMove).x));
      if((*previousMove).y != MAX_VAL)
        lastMove.y = *(new double((*previousMove).y));
      if((*previousMove).z != MAX_VAL)
        lastMove.z = *(new double((*previousMove).z));
    }
    
    if(currentMove)  { 
      //Serial.print("\n G?!");
      if((*currentMove).x != MAX_VAL)
        to.x = *(new double((*currentMove).x));
      if((*currentMove).y != MAX_VAL)
        to.y = *(new double((*currentMove).y));
      if((*currentMove).z != MAX_VAL)
        to.z = *(new double((*currentMove).z));
      if((*currentMove).f != MAX_VAL)
        feedrate = *(new double((*currentMove).f));
        
      // Set Laser Power
      if((*currentMove).s != MAX_VAL)
      {
        lastLaserPWR = *(new double((*currentMove).s));
        laserChanged = true;
      }

      //Serial.print("\n Start Move G");Serial.print((*currentMove).code);Serial.print(" @");Serial.print(nanos());Serial.print("\n");
      if((*currentMove).code != 0) { // G0 means no calculation and no interpolation as feedrate is ignored and laser is expected off.
        //Serial.print("\nInterpreted as NOT G0");
        laser.setLaserPWM(lastLaserPWR);
        //Serial.print("\n LaserPower");Serial.print(lastLaserPWR);Serial.print(" Move laserPower:");Serial.print((*currentMove).s);
        startNanos = _now;      
        distx = to.x-lastMove.x;
        disty = to.y-lastMove.y;
        distz = to.z-lastMove.z; // not used.....
        calculateMoveLengthNanos(distx, disty, feedrate, &((*currentMove).moveLengthNanos));
        endNanos = startNanos + (*currentMove).moveLengthNanos;
      }
      else
      {
        //Serial.print("\nInterpreted as G0");
        laser.setLaserPWM(0);
      }
      laser.handleLaser();
      beginNext = false;
    }
  }
  
  //interpolate  
  if(currentMove && (_now > endNanos || (*currentMove).code == 0))  //We are out of time or G0
  {
    laser.handleLaser();
    //Serial.print("\n End Of Interpolated Move:\n ");Serial.print(interpolCnt);//Serial.print(" interpolations where calculated \n - GALVO - Go to coord X:");Serial.print(to.x);
    //Serial.print("\n value x:");
    //Serial.print((int)map(to.x, 0.0,250.0, 0,65535)+0.5);
    //Serial.print("coord Y:");
    //Serial.print(to.y);
    //Serial.print(" value y:");
    //Serial.print((int)map(to.y, 0.0,250.0, 0,65535)+0.5);    
    galvo.goTo(map(to.x, 0.0,250.0, 0,65535)+0.5, map(to.y, 0.0,250.0, 0,65535)+0.5); //Make sure to hit the commanded position
    //Serial.print("\n Completed @");Serial.print(_now);Serial.print("\n");
    beginNext = true;
    interpolCnt=0;
    return;
  }
  else if (currentMove)
  {


    distx = to.x-lastMove.x;
    disty = to.y-lastMove.y;
    distz = to.z-lastMove.z; // not used.....
    //Serial.print("\n Interpolate: DBG - lastMove X:");Serial.print(lastMove.x);Serial.print(" to.X::");Serial.print(to.x);
    uint64_t t = (*currentMove).moveLengthNanos; 
    double fraction_of_move = (double)(_now-startNanos)/t;
    double x = (lastMove.x + (distx*fraction_of_move));
    double y = (lastMove.y + (disty*fraction_of_move));
    interpolCnt++;
    //Serial.print("\n Interpolate: GALVO - Go to x:");Serial.print(x);Serial.print(" / ");Serial.print(to.x);Serial.print(" Y:");Serial.print(y);Serial.print(" / ");Serial.print(to.y);
    galvo.goTo( map(x, 0.0,250.0, 0,65535)+0.5, map(y, 0.0,250.0, 0,65535)+0.5 );
    return ;
  }
  else 
  {
    //Serial.println("Idle");
  }
  return;
}

void loop() {  
  delay(1);
  if(itcnt>2)
  {
    serialReciever.handleSerial();
    process();
    
#ifdef LASER_IS_SYNRAD
    laser.handleLaser();    
#endif

    itcnt = 0;
  }
  else
    process();
  itcnt++;
}


