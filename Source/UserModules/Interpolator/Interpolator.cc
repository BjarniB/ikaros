//
//	Interpolator.cc		This file is a part of the IKAROS project
//
//    Copyright (C) 2012 <Author Name>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    See http://www.ikaros-project.org/ for more information.
//
//  This example is intended as a starting point for writing new Ikaros modules
//  The example includes most of the calls that you may want to use in a module.
//  If you prefer to start with a clean example, use he module MinimalModule instead.
//

#include "Interpolator.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace ikaros;



void
Interpolator::Init()
{   

  Bind(tau,"tau");

  input_ticks = GetInputMatrix("INPUT_TICKS");
  input_values = GetInputMatrix("INPUT_VALUES");

  input_size_y = GetInputArray("SIZE_Y");
  input_size_x = GetInputArray("SIZE_X");

  input_command = GetInputArray("COMMAND");

  output_array = GetOutputArray("OUTPUT");
  output_array_size = GetOutputSize("OUTPUT");

  output_test = GetOutputArray("OUTPUT_TEST");
  output_test2= GetOutputArray("OUTPUT_TEST2");

  tick = 0;

  current_state = eStart;

  /* Coefficients for Matrix M */
  M11 = 0.0;
  M12 = 1.0;
  M13 = 0.0;
  M14 = 0.0;
  M21 =-tau;
  M22 = 0.0;
  M23 = tau;
  M24 = 0.0;
  M31 = (2*tau);
  M32 = (tau-3);
  M33 = (3-2*tau);
  M34 = -tau;
  M41 = -tau;
  M42 = (2-tau);
  M43 = (tau-2);
  M44 = tau;
}



Interpolator::~Interpolator()
{
    // Destroy data structures that you allocated in Init.
}



void
Interpolator::Tick()
{

  Command cmd = (Command)input_command[0];

  switch(current_state){
    case eStart:
      current_state = eInterpolating;
      break;
    case eInterpolating:
      if(cmd == ePause){
        // perhaps use a bool attribute to finish playing the current motion before changing state
        current_state = ePaused;
        printf("switching to pause\n");
        //exit(0);
      }else{

        if (cmd > 0)  {
          if(debugmode)
            printf("cmd is %i", cmd);
          
          tick = cmd; 
        }

        //USE CATMULL ROM WITH DEFINED TAU MATRIX
        copy_array(output_array, catmullRomSpline(), (int)input_size_y[0]);
        //copy_array(output_test, LinearInterpolation(tick), (int)input_size_y[0]);
      }
      break;
    case ePaused:
      if(cmd == eInterpolate)
        current_state = eInterpolating;
    default:
      break;
  }

  if(debugmode){
    print_array("output", output_test2, (int)input_size_y[0]);
    printf("sizes: %i, %i \n", (int)input_size_x[0],(int)input_size_y[0]);
  }

  tick++;

  if(debugmode)
    printf("TICK : %i\n", tick);
  
}

// Catmull Rom cubic interpolation, uses the M values defined above controlled by the tau value
float* 
Interpolator::catmullRomSpline() {

  float* ret = create_array((int)input_size_y[0]);

  for(int i = 0; i < input_size_y[0]; ++i){

    // point indexes
    int v0,v1,v2,v3;
    v2 = FindP2(i);

    if(debugmode)
      printf("V2: %i, %f \n", v2, input_values[i][v2]);

    if(input_values[i][v2] == -1){
      tick = 0;
      break;
    }

    if(v2 == 1){
      v0 = 0;
      v1 = 0;
      v3 = 2;
    }else if(input_values[i][v2+1] == -1){
      v0 = v2-2;
      v1 = v2-1;
      v3 = v2;
    }else{
      v0 = v2-2;
      v1 = v2-1;
      v3 = v2+1;
    }

    // where along the normalized distance
    float x = (tick-input_ticks[i][v1]) / (input_ticks[i][v2]-input_ticks[i][v1]);

    float c1,c2,c3,c4;
    c1 = M12*input_values[i][v1];
    c2 = M21*input_values[i][v0] + M23*input_values[i][v2];
    c3 = M31*input_values[i][v0] + M32*input_values[i][v1] + M33*input_values[i][v2] + M34*input_values[i][v3];
    c4 = M41*input_values[i][v0] + M42*input_values[i][v1] + M43*input_values[i][v2] + M44*input_values[i][v3];
    
    ret[i] = ((c4*x + c3)*x +c2)*x + c1;
  }

  return ret;
}

// Help function to find the target point index
int 
Interpolator::FindP2(int index_y){
  int index = 0;
  for(int i = 0; i < input_size_x[0]; ++i){
    index = i;
    if(input_ticks[index_y][i] > tick || input_ticks[index_y][i] == -1){
      break;
    }
  }
  return index;
}

// Linear interpolation function
float*
Interpolator::LinearInterpolation(int tick)
{


  float* ret = create_array((int)input_size_y[0]);

      //iterate through all curves
  for (int i = 0; i < input_size_y[0]; i++) {


    for (int n = 0; n < input_size_x[0]; n++) {
      if(debugmode)
        printf("check %i %i %f \n",i,n,input_ticks[i][n]);
      if(input_values[i][n] == -1){
        tick = 0;
        break;
      }

     if(tick < input_ticks[i][n]) {

       int prev = n-1;
       int next = n;

       float val = input_values[i][prev];
       val += ((input_values[i][next]-input_values[i][prev])/(input_ticks[i][next]-input_ticks[i][prev]))*(tick-input_ticks[i][prev]);
       ret[i] = val;
       break;

     }
   }
 }

 return ret;
}

// Install the module. This code is executed during start-up.

static InitClass init("Interpolator", &Interpolator::Create, "Source/UserModules/Interpolator/");


