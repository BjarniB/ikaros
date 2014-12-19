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
    // Parameters
    Bind(port, "port");
    Bind(debugmode, "debug");
    Bind(a,"a");
    Bind(b,"b");
    Bind(c,"c");
    Bind(d,"d");
    Bind(destport,"destport");
    Bind(packet_size, "packet_size");

    // Inputs
    input_matrix_pos = GetInputMatrix("IN_VALUE");
    input_matrix_tick = GetInputMatrix("IN_TICK");
    input_matrix_sizeX = GetInputArray("IN_SIZE_X");
    input_matrix_sizeY = GetInputArray("IN_SIZE_Y");
    sync_in = GetInputArray("SYNC_IN");

    // Outputs
    output_matrix_pos = GetOutputMatrix("OUT_VALUE");
    output_matrix_tick = GetOutputMatrix("OUT_TICK");
    size_param_x = GetOutputArray("SIZE_X");
    size_param_y = GetOutputArray("SIZE_Y");
    output_command = GetOutputArray("COMMAND");

    printf( "creating socket on port %d\n", port );

    if ( !socket.Open( port ) )
    {
        printf( "failed to create socket!\n" );
        exit(0);
    }

    dest = mAddress(a,b,c,d,destport);

    tick = 0;
    once = true;

    current_state = eStart;
}



SocketModule::~SocketModule()
{
    ShutdownSockets();
}


void
SocketModule::Tick()
{
    switch(current_state){
        case eStart:
            current_state = eWaitingToSend;
            break;
        case eWaitingToSend:
            if(sync_in[0] == 1.0f){
                current_state = eSending;
            }
            break;
        case eSending:
            SendData();
            break;
        case eReceiving:
            ReceiveData();
        default:
            break;
    }
}


// Send data as tick:pos tick:pos#tick:pos tick:pos
// # delimits different servos : delimits tick and position value
void
SocketModule::SendData(){
    std::string data = SetupSendData(input_matrix_tick, input_matrix_pos, (int)input_matrix_sizeX[0], (int)input_matrix_sizeY[0]);

    printf("%s\n", data.c_str());

    bool send = socket.Send(dest, data.c_str(), data.size());

    if(send)
        current_state = eReceiving;
    else
        printf("Failed to send data, trying again next tick\n");
}

// ReceiveData function that takes a package from the port and parses it in different ways
void 
SocketModule::ReceiveData(){
    mAddress sender;
    unsigned char buffer[packet_size];
    int bytes_read = socket.Receive( sender, buffer, sizeof( buffer ) );
    

    if ( !bytes_read ){}
    else{   
        if(debugmode){
            printf( "received packet from %d.%d.%d.%d:%d (%d bytes) (%s)\n", 
                sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), 
                sender.GetPort(), bytes_read , buffer);
        }

        // If a flag is sent
        if(buffer[0] == '-'){
            if(debugmode)
                printf("parsing\n");

            output_command[0] = ParseFlag((char*)buffer,sizeof(buffer));    // Parse flag and output as command
            
            if(debugmode)
                printf("parsing done %f\n", output_command[0] );

        }
        // Package is flagged as P
        else if(buffer[0] == 'P'){
            if(debugmode)
                printf("parsing\n");
            
            output_command[0] = ParsePlayTick((char*)buffer,sizeof(buffer));    // Parse flag and output as command
            
            if(debugmode)
                printf("parsing done %f\f", output_command[0] );
                        
        }
        // Parse as usual if not flagged
        else{
            char * buf = (char*)buffer;

            size_param_x[0] = sizeof(buffer);
            size_param_y[0] = input_matrix_sizeY[0];

            if(debugmode)
                printf("%s ; %i, %i\n", buf, (int)size_param_x[0], (int)size_param_y[0]);

            copy_matrix(output_matrix_pos, ParseValue2(buf,(int)size_param_x[0], (int)size_param_y[0]), (int)size_param_x[0], (int)size_param_y[0]);
            copy_matrix(output_matrix_tick, ParseValue1(buf,(int)size_param_x[0], (int)size_param_y[0]), (int)size_param_x[0], (int)size_param_y[0]);
        }
    }   
}

// Install the module. This code is executed during start-up.

    static InitClass init("SocketModule", &SocketModule::Create, "Source/UserModules/SocketModule/");


