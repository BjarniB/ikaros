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



    // 2 output matrix, en matrix med servo för varje rad och position i column, en för servo per rad och ticks i column

     // char in[] = "120:0 200:200 120:400#90:0 50:200 90:400";

     // char * ins = in;

     // int s = CalcSizeX(in, sizeof(in));

     // printf("Sizeof: %i, size: %i", sizeof(in), s);

     // exit(0);

//     size_param_x[0] = sizeof(in);
//     size_param_y[0] = 2;

// printf("changing matrix\n");

//     copy_matrix(output_matrix_pos, ParseValue1(ins,2,sizeof(in)), (int)size_param_x[0], (int)size_param_y[0]);
//     copy_matrix(output_matrix_tick, ParseValue2(ins,2,sizeof(in)), (int)size_param_x[0], (int)size_param_y[0]);

// printf("chaning done\n");
    // print_matrix("vals", vals, sizeof(in), 2);

    // print_matrix("vals2", vals2, sizeof(in), 2);

    //int * size;
    //const char * send = SetupSendData(vals, vals2, sizeof(in), 2, size);


    //printf("Size: %i, %i\n", sizeof(send), (int)size);


    // TODO loopa igenom en char parameter och skapa outputs som definieras
    //printf("%s : %i\n", output_list,sizeof(output_list));
    //Parse output_list and add outputs
    //TODO flytta skapandet av outputs till constructorn
    // char buf[10] = "";
    // int i = 0, c = 0;
    // while(true){
    //     if(output_list[i] == ' ' || output_list[i] == '\0'){
    //         buf[c] = '\0';
    //         printf("%s\n", buf);
    //         AddOutput(buf,GetInputSizeX("INPUT"),GetInputSizeY("INPUT"));
    //         output_vector.push_back(GetOutputArray(buf));
    //         c = 0;
    //     }else{
    //         buf[c] = output_list[i];
    //         c++;
    //     }
    //     printf("%i\n", i);

    //     if(output_list[i] == '\0')
    //         break;
    //     i++;
    // }

    //sync_in = GetInputArray("SYNC_IN");

    printf( "creating socket on port %d\n", port );

    if ( !socket.Open( port ) )
    {
        printf( "failed to create socket!\n" );
        exit(0);
    }

    dest = mAddress(a,b,c,d,port);

    tick = 0;

}



SocketModule::~SocketModule()
{
    ShutdownSockets();
}


void
SocketModule::Tick()
{
    if(sync_in[0] == 1.0f){

        print_matrix("ticks", input_matrix_tick, (int)input_matrix_sizeX[0], (int)input_matrix_sizeY[0]);

        print_matrix("values", input_matrix_pos, (int)input_matrix_sizeX[0], (int)input_matrix_sizeY[0]);

        printf("Sizes: %i, %i\n", (int)input_matrix_sizeX[0], (int)input_matrix_sizeY[0]);

        int * s;
        std::string data = SetupSendData(input_matrix_tick, input_matrix_pos, (int)input_matrix_sizeX[0], (int)input_matrix_sizeY[0]);


        printf("%s\n", data.c_str());

        bool send = socket.Send(dest, data.c_str(), sizeof(data));

        if(send)
            printf("True\n");
        else
            printf("False\n");

        exit(0);
    }
    //TODO Implement states eStart, eWaitingToSend, eSending, eReceiving
    // waiting for sync signal input for changing states from eWaitingToSend to eSending
    ReceiveData();

}


// Send data as tick:pos tick:pos#tick:pos tick:pos
// # delimits different servos : delimits tick and position value
void
SocketModule::SendData(void * data){
    //TODO structure data in some special manner for interface interpretation
    socket.Send( dest, data, sizeof(data) );
}

void 
SocketModule::ReceiveData(){
    mAddress sender;
    unsigned char buffer[packet_size];
    int bytes_read = socket.Receive( sender, buffer, sizeof( buffer ) );
    

    if ( !bytes_read ){}
        else{   
            printf( "received packet from %d.%d.%d.%d:%d (%d bytes) (%s)\n", 
                sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), 
                sender.GetPort(), bytes_read , buffer);

             size_param_x[0] = sizeof(buffer);
             size_param_y[0] = 3;

             char * buf = (char*)buffer;

             printf("%s ; %i, %i\n", buf, (int)size_param_x[0], (int)size_param_y[0]);

             //float ** n1 = ParseValue1(buf,(int)size_param_x[0], (int)size_param_y[0]);
             //float ** n2 = ParseValue2(buf,(int)size_param_x[0], (int)size_param_y[0]);

             //printf("Pare DONE\n");

  //print_matrix("ticks", n1, (int)size_param_x[0], (int)size_param_y[0]);

  //print_matrix("values", n2, (int)size_param_x[0], (int)size_param_y[0]);

             //exit(0);

            printf("changing matrix\n");

            copy_matrix(output_matrix_pos, ParseValue2(buf,(int)size_param_x[0], (int)size_param_y[0]), (int)size_param_x[0], (int)size_param_y[0]);
            copy_matrix(output_matrix_tick, ParseValue1(buf,(int)size_param_x[0], (int)size_param_y[0]), (int)size_param_x[0], (int)size_param_y[0]);

            printf("chaning done\n");
        }

    }



// Install the module. This code is executed during start-up.

    static InitClass init("SocketModule", &SocketModule::Create, "Source/UserModules/SocketModule/");


