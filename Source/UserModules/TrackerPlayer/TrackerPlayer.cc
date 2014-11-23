#include "TrackerPlayer.h"
#include<iostream>
#include <math.h>

using namespace ikaros;
using namespace std;


void
TrackerPlayer::Init()
{

	Bind(playOnRead, "playOnRead");
	ticks = 0;
	stopTick = 0;
	amtServo = 3;

	readingFile = true;
	
	lineSum = 0;
 	reachedFileEnd = false;

 	switcher = false;


	input_pos_array = GetInputArray("INPUT_POS");
	input_pos_array_size = GetInputSize("INPUT_POS");

	input_file_array = GetInputArray("INPUT_FILE");
	input_file_array_size = GetInputSize("INPUT_FILE");

	

	output_pos_array = GetOutputArray("OUTPUT_POS");
	output_pos_array_size = GetOutputSize("OUTPUT_POS");

	output_torque_array = GetOutputArray("OUTPUT_TORQUE");
	output_torque_array_size = GetOutputSize("OUTPUT_TORQUE");



	// avoid getting dynamixel error message for not setting position
	for(int i = 0; i < amtServo; i++) {
		output_torque_array[i] = 0;
		output_pos_array[i] = 40;

	}



}



TrackerPlayer::~TrackerPlayer()
{

	destroy_array(input_file_array);
	destroy_array(input_pos_array);
	destroy_array(output_pos_array);

}



void
TrackerPlayer::Tick()
{ 

	// read angular data from file
	if (readingFile) {

		printf("\ninput kropp: %f", input_file_array[0]);
		printf("\ninput axel: %f", input_file_array[1]);
		printf("\ninput armbåge: %f", input_file_array[2]);

		if(ticks == 1) {
			for (int i = 0; i < amtServo; i++) {
				startPos[i] = input_pos_array[i];
				startOffset[i] = startPos[i]-input_file_array[i];
			}
			
			
			
		}
		
		lineSum = 0;

		for (int i = 0; i < amtServo; i++)  {
			tickPos[i][ticks] = input_file_array[i];

			lineSum += input_file_array[i];

			if(playOnRead == 1 && ticks > 1)  {
				output_pos_array[i] = tickPos[i][ticks]+startOffset[i];
				output_torque_array[i] = 1;

			}

			
		}

		if(ticks > 0 && lineSum == 0)
			reachedFileEnd = true;


		// if reached end of file
		if (reachedFileEnd) {

			//set offset
			for(int i = 0; i < amtServo; i++) {
				//save current servo position as starting point

				startPos[i] = input_pos_array[i];
				startOffset[i] = startPos[i]-tickPos[i][1];
				output_torque_array[i] = 1;
				output_pos_array[i] = startPos[i];
				
				
			}


			stopTick = ticks;
			readingFile = false;
			ticks = 0;

		}


	// play motions if finished reading file
	} else {

		//printf("\ninput kropp: %f", input_file_array[0]);
		//printf("\ninput axel: %f", input_file_array[1]);
		//printf("\ninput armbåge: %f", input_file_array[2]);

		if (ticks >= stopTick) {
		//ticks = 0;
			printf("\n ######REACHED END##########\n" );
			exit(0);

		}

		for (int i = 0; i < amtServo; i++) {

				output_pos_array[i] = ((tickPos[i][ticks]+tickPos[i][ticks+1])/2)+startOffset[i];

			printf("\n sending - servo:tick:pos - %i: %i %f", i, ticks,output_pos_array[i]);
		}

		//if (!switcher) {
		//	ticks--;
		//	printf("\n ticks--");
		//}
		//switcher = !switcher;


	}

	ticks++;
	

}

// Install the module. This code is executed during start-up.

static InitClass init("TrackerPlayer", &TrackerPlayer::Create, "Source/UserModules/TrackerPlayer/");


