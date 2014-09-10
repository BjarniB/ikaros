#include "RobotToPos.h"
#include<iostream>
#include <math.h>

using namespace ikaros;
using namespace std;


void
RobotToPos::Init()
{

    Bind(angle, "angle");


    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");


    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");


  angle = 30.0;
}



RobotToPos::~RobotToPos()
{
}



void CalcAngle(float x, float y){

	float angle = 	atan(y/x);
	angle *= (180/ M_PI);
	cout << "x: " << x << endl;

	if ( x < 0 )
		angle = 180 - angle;

	
cout << "Angle: " << angle << endl;
	

return;

}
void AddCords(){
    float x = 0.0f, y = 0.0f;
    cout << "Please enter x- and y-cords "
	<< "to add separated by a white space:" << endl; 
    cin >> x >> y;
    cout << x << " + " << y << endl;
	CalcAngle(x, y);
    return;
}

void
RobotToPos::Tick()
{
	if (input_array[0]!=angle){
}
output_array[0] = angle ;

}



// Install the module. This code is executed during start-up.

static InitClass init("RobotToPos", &RobotToPos::Create, "Source/UserModules/RobotToPos/");


