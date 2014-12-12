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

  input_ticks = GetInputMatrix("INPUT_TICKS");
  input_values = GetInputMatrix("INPUT_VALUES");

  input_size_y = GetInputArray("SIZE_Y");
  input_size_x = GetInputArray("SIZE_X");

  output_array = GetOutputArray("OUTPUT");
  output_array_size = GetOutputSize("OUTPUT");

  tick = 0;

}



Interpolator::~Interpolator()
{
    // Destroy data structures that you allocated in Init.
  destroy_matrix(input_ticks);
  destroy_matrix(input_values);
  destroy_array(output_array);
}



void
Interpolator::Tick()
{

  printf("TICK interpolate \n");

  //print_matrix("ticks", input_ticks, (int)input_size_x[0], (int)input_size_y[0]);

  //print_matrix("values", input_values, (int)input_size_x[0], (int)input_size_y[0]);

  printf("sizes: %i, %i \n", (int)input_size_x[0],(int)input_size_y[0]);

  copy_array(output_array, GetInterpolation(tick),(int)input_size_y[0]);

  
  tick++;

  printf("TICK : %i\n", tick);

  // play from beging if reached end
  if (tick >= 400)
    tick = 0;
  
}


float*
Interpolator::GetInterpolation(int tick)
{


  float* ret = create_array((int)input_size_y[0]);


      // linear interpolation algorithm

      //iterate through all curves

   //int amtCurve = 2;
   //  int amtTicks = 4;

  for (int i = 0; i < input_size_y[0]; i++) {


    for (int n = 0; n < input_size_x[0]; n++) {
           printf("check %i %i %f \n",i,n,input_ticks[i][n]);
      if(input_values[i][n] == -1){
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


