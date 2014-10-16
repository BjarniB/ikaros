//
//	MovingAverage.cc		This file is a part of the IKAROS project
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

#include "MovingAverage.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
MovingAverage::Init()
{
    
    Bind(buffer_length, "buffer_length");
    
    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");

    // Do the same for the outputs

    output_array = GetOutputArray("OUTPUT");


    // Allocate some data structures to use internaly
    // in the module
    avg_factor = 1.0/buffer_length;
    buffer = new std::deque<float>[input_array_size];

}



MovingAverage::~MovingAverage()
{
    // Destroy data structures that you allocated in Init.
    delete[] buffer;

    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
}



void
MovingAverage::Tick()
{
    
    for (int i = 0; i < input_array_size; ++i)
    {
        buffer[i].push_back(input_array[i]);
        // fill buffer before doing avg
        if(buffer[i].size() == buffer_length)
        {
            float sum=0;
            for (int j = 0; j < buffer[i].size(); ++j)
                sum+=buffer[i].at(j);
            output_array[i] = avg_factor * sum;
            buffer[i].pop_front();
        }
    }
}



// Install the module. This code is executed during start-up.

static InitClass init("MovingAverage", &MovingAverage::Create, "Source/UserModules/MovingAverage/");


