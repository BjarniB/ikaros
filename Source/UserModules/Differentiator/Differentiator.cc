//
//	Differentiator.cc		This file is a part of the IKAROS project
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

#include "Differentiator.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
Differentiator::Init()
{
    // To get the parameters from the IKC file, use the Bind
    // function for each parameter. The parameters are initialized
    // from the IKC and can optionally be changed from the
    // user interface while Ikaros is running. If the parameter is not
    // set, the default value will be used instead.
    
    Bind(gain, "gain");
    Bind(reset_period, "reset_period");
    Bind(rectify, "rectify");
    
    // This is were we get pointers to the inputs and outputs

    // Get a pointer to the input INPUT1 and its size which we set
    // to 10 above
    // It does not matter whether a array of array is connected
    // to the inputs. We will treat it an array in this module
    // anyway.

    
    // Get pointer to a array and treat it as a array. If an array is
    // connected to this input, size_y will be 1.

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");

    // Do the same for the outputs

    
    output_array = GetOutputArray("OUTPUT");
    
    // Allocate some data structures to use internaly
    // in the module

    // Create an array with ten elements
    // To access the array use internal_array[i].

    
    // Create a array with the same size as INPUT2
    // IMPORTANT: For the array the sizes are given as X, Y
    // which is the OPPOSITE of ROW, COLUMN.

    prev_val = create_array(input_array_size);
    //printf("sizex = %i, sizey = %i\n", input_array_size_x, input_array_size_y);
    // To acces the array use internal_array[y][x].
    //
    // IMPORTANT: y is the first index and x the second,
    //
    // It is also possible to use the new operator to
    // create arrays, but create_array and create_matix
    // should be used to make sure that memeory is
    // allocated in a way that is suitable for the math
    // library and fast copying operations.
    //first = true;
    tick = 0;
}



Differentiator::~Differentiator()
{
    // Destroy data structures that you allocated in Init.

    destroy_array(prev_val);

    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputarray etc.
}



void
Differentiator::Tick()
{
    // skip first 
    // TODO make skip first and second to parameters
    

    if(tick > 1)
    {
        subtract(output_array, input_array, prev_val, input_array_size);
        multiply(output_array, gain, input_array_size);
        // TODO implement rectification
        
        //for (int i = 0; i < input_array_size; ++i)
        //    printf("Differentiator out %i, %f\n", i, output_array[i]);
    }
    
    if(tick > 0)
    {
        //for (int i = 0; i < input_array_size; ++i)
        //    printf("Differentiator in %i, %f\n", i, input_array[i]);
        copy_array(prev_val, input_array, input_array_size);
    }
    tick++;
    
}



// Install the module. This code is executed during start-up.

static InitClass init("Differentiator", &Differentiator::Create, "Source/UserModules/Differentiator/");


