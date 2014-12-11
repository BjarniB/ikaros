//
//	Player.cc		This file is a part of the IKAROS project
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

#include "Player.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
Player::Init()
{
	Bind(debugmode, "debug");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");

    sync_in = GetInputArray("SYNC_IN");

    socket_input = GetInputArray("SOCKET_INPUT");
    socket_input_size = GetInputSize("SOCKET_INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_torque = GetOutputArray("TORQUE");
    output_pos = GetOutputArray("POSITION");
    output_speed = GetOutputArray("SPEED");

}



Player::~Player()
{
}



void
Player::Tick()
{
    //TODO use states to determine what to do
    //TODO every tick take the input from socket_input and parse it into the outputs
	if(debugmode)
	{
		// print out debug info
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("Player", &Player::Create, "Source/UserModules/Player/");


