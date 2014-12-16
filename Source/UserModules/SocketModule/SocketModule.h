//
//	SocketModule.h		This file is a part of the IKAROS project
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

#ifndef SocketModule_
#define SocketModule_

#include "IKAROS.h"
#include "Net.h"
#include "Parse.h"
#include <vector>

using namespace net;
using namespace std;
using namespace parse;

class SocketModule: public Module
{
public:
    static Module * Create(Parameter * p) { return new SocketModule(p); }

    SocketModule(Parameter * p) : Module(p) {}
    virtual ~SocketModule();

    void 		Init();
    void 		Tick();
    void        SendData(void * data);
    void        ReceiveData();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array;

    float **    input_matrix_pos;
    float **    input_matrix_tick;
    float *     input_matrix_sizeX;
    float *     input_matrix_sizeY;

    float **    output_matrix_pos;
    float **    output_matrix_tick;
    float *     size_param_x;
    float *     size_param_y;


    float *     output;
    std::vector<float*>      output_vector;

    float *     sync_in;

    //parameter values
    bool        debugmode;
    mSocket     socket;
    int         port;
    int         packet_size;

    // Destination parameters
    int         a,b,c,d;
    int         destport;
    mAddress    dest;

    int         tick;

    bool once;
    // TODO add states
};

#endif
