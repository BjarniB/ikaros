//
//	Reset.cc		This file is a part of the IKAROS project
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

#include "Reset.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

const float cRecording = 0.f;
const float cPlay = 1.f;

void
Reset::Init()
{
	Bind(debugmode, "debug");    
    Bind(delay, "delay");    
    Bind(equaltolerance, "equaltolerance");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    state = GetInputArray("STATE");
    
    sync = GetOutputArray("SYNC");
    sync[0] = 0.f;
    output_array = GetOutputArray("OUTPUT");

    internal_array = create_array(input_array_size);
}



Reset::~Reset()
{
    // Destroy data structures that you allocated in Init.
    destroy_array(internal_array);
}



void
Reset::Tick()
{
    // bug: delay can overflow and begin at 0 again
    if(state[0] == cRecording && !delay--)
    {
        copy_array(internal_array, input_array, input_array_size);
        if(debugmode)
            printf("reset got %f\n", input_array[0]);
        //done = true;
        //delay--;
    }
	else if (state[0] == cPlay) 
    {
        copy_array(output_array, internal_array, input_array_size);
        bool ok=true;
        for (int i = 0; i < input_array_size; ++i)
            ok = ok && equal(input_array[i], internal_array[i], equaltolerance);
        if(ok)
            sync[0] = cPlay;
    }
    // all other values do nothing
    if(debugmode)
	{
		// print out debug info
        for (int i = 0; i < input_array_size; ++i)
            printf("reset %i, %f, intput=%f, sync=%f\n, delay=%i\n", i, internal_array[i], input_array[0], sync[0], delay );
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("Reset", &Reset::Create, "Source/UserModules/Reset/");


