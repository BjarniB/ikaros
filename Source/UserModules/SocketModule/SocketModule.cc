//
//	SocketModule.cc		This file is a part of the IKAROS project
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

#include "SocketModule.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace net;
using namespace ikaros;


void
SocketModule::Init()
{
    Bind(port, "port");
	Bind(debugmode, "debug");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");


    internal_array = create_array(10);

    printf( "creating socket on port %d\n", port );

    if ( !socket.Open( port ) )
    {
        printf( "failed to create socket!\n" );
        exit(0);
    }

    tick = 0;

}



SocketModule::~SocketModule()
{
    // Destroy data structures that you allocated in Init.
    destroy_array(internal_array);
}



void
SocketModule::Tick()
{
    mAddress sender;
    unsigned char buffer[1024];
    int bytes_read = socket.Receive( sender, buffer, sizeof( buffer ) );
    
    if(debugmode)
    {
        if ( !bytes_read ){}
        else{   
            printf( "received packet from %d.%d.%d.%d:%d (%d bytes) (%s)\n", 
                sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), 
                sender.GetPort(), bytes_read , buffer);

//            Packet p = Packet(buffer);
  //          p.ParsePacket();
        }
	}

    //TODO fråga chrstian om char som outputs
}





// Install the module. This code is executed during start-up.

static InitClass init("SocketModule", &SocketModule::Create, "Source/UserModules/SocketModule/");


