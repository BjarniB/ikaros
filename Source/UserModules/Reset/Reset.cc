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
const float cNull = 0.f;

void
Reset::Init()
{
	Bind(debugmode, "debug");    
    Bind(delay, "delay");    
    Bind(equaltolerance, "equaltolerance");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    command = GetInputArray("COMMAND");    
    
    sync = GetOutputArray("SYNC");
    sync[0] = 0.f;
    output_array = GetOutputArray("OUTPUT");

    internal_array = create_array(input_array_size);
    current_state = eStart;
}



Reset::~Reset()
{
    // Destroy data structures that you allocated in Init.
    destroy_array(internal_array);
}

/**
State transitions:

start -> off

*/
void
Reset::Tick()
{
    Command cmd = (Command)command[0];

    switch (current_state)
    {
        case eStart:
            //printf("start, ");
            if(cmd==eStore)
            {
                copy_array(internal_array, input_array, input_array_size);
                current_state = eLoaded;
            }
            break;
        case eLoaded:
            //printf("loaded, ");
            if(cmd==eReset)
            {
                copy_array(output_array, internal_array, input_array_size);
                current_state = eResetting;
            }
            else if(cmd==eStore)
                copy_array(internal_array, input_array, input_array_size);
            break;
        case eResetting:
            //printf("resetting, ");
            if (equal(input_array, internal_array, input_array_size, equaltolerance))
            {
                current_state = eAt_Reset;
                set_array(output_array, cNull, input_array_size);   
                sync[0] = 1.f;
            }
            if (cmd==eStop)
            {
                set_array(output_array, cNull, input_array_size);
                current_state = eLoaded;
            }
            break;
        case eAt_Reset:
            //printf("at reset, ");
            if (!equal(input_array, internal_array, input_array_size, equaltolerance))
            {
                current_state = eLoaded;
                sync[0] = 0.f;
            }
            if(cmd == eStore)
              copy_array(internal_array, input_array, input_array_size);
            break;
        default:
            break;
    }
    // all other values do nothing
    if(debugmode)
	{
		// print out debug info
        for (int i = 0; i < input_array_size; ++i)
            printf("reset: i=%i, intarray=%f, input=%f, command=%f, sync=%f, current_state=%i\n", i, internal_array[i], input_array[i], command[0], sync[0], current_state );
        if(cmd==eStore) printf("store\n");
        if(cmd==eReset) printf("reset\n");
        if(cmd==eStop) printf("stop\n");
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("Reset", &Reset::Create, "Source/UserModules/Reset/");


