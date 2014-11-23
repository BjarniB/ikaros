//
//	Clamp.cc		This file is a part of the IKAROS project
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

#include "Clamp.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
Clamp::Init()
{
    Bind(min_out, "min_out");
    Bind(max_out, "max_out");
    Bind(tolerance, "tolerance");
    Bind(offset_in, "offset_in");
    Bind(tristate, "tristate");
	Bind(debugmode, "debug");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");

    output_array = GetOutputArray("OUTPUT");
    //output_array_size = GetOutputSize("OUTPUT");

    internal_array = create_array(10);
}



Clamp::~Clamp()
{
    // Destroy data structures that you allocated in Init.
    destroy_array(internal_array);
}



void
Clamp::Tick()
{
    // TODO rewrite to use array methods, dont need loop
	for (int i = 0; i < input_array_size; ++i)
    {
        if(equal(input_array[i], offset_in, tolerance))
            if(tristate)
                output_array[i] = offset_in;
            else 
                continue;
        else if(input_array[i] < offset_in)
            output_array[i] = min_out;
        else
            output_array[i] = max_out;
        if(debugmode)
        {
            printf("output %i=%f\n", i, output_array[i] );
        }
    }
    
}



// Install the module. This code is executed during start-up.

static InitClass init("Clamp", &Clamp::Create, "Source/UserModules/Clamp/");


