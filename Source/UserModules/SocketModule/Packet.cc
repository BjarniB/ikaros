#include "Packet.h"

Packet::Packet( unsigned char* buf ){
	buffer = buf;
	values1 = new std::vector<float>[sizeof(buffer)];
	values2 = new std::vector<float>[sizeof(buffer)];
}

Packet::~Packet(){
	delete[] values;
}

void
Packet::ParsePacket(){
	std::string val = "";
	for(int i = 0; i < sizeof(buffer); i++){
		switch(buffer[i]){
			case 'P' || 'T' || 'V':
				type = buffer[i];
				id = buffer[i+2];
				printf("type: %c, id: %i");
				i += 2;
				break;
			case ' ':
				value2.push_back(strtof(val,NULL));
				printf("value2 pushback %s", val);
				val = "";
				break;
			case ':':
				value1.push_back(strtof(val,NULL));
				printf("value2 pushback %s", val);
				val = "";
				break;
			default:
				val += buffer[i];
				break;
		}
	}
}
