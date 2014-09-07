//
//	Sigmoid.cc		This file is a part of the IKAROS project
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

#include "Sigmoid.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
Sigmoid::Init()
{
    
    Bind(target, "target");
    Bind(alpha, "alpha");
    Bind(shift, "shift");
    input_array_size = GetInputSize("INPUT");
    input_array = GetInputArray("INPUT"); 
    // Do the same for the outputs

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    t=0;
    offset = 0.0;
    scale = 1.0;
}



Sigmoid::~Sigmoid()
{
}



void
Sigmoid::Tick()
{
    if(input_array[0]!=target)
    {
        t=0.0;
        scale = input_array[0]-target;
        offset = target;
        target = input_array[0];
    }
    //output_array[0] = target*1.0/(1+exp(-alpha*t-shift));
    output_array[0] = scale/(1+exp(-alpha*t-shift)) + offset;//pow(2.71828,-t);
    //printf("%f,%f\n", output_array[0],t);
    t+=1.0;
}



// Install the module. This code is executed during start-up.

static InitClass init("Sigmoid", &Sigmoid::Create, "Source/UserModules/Sigmoid/");


