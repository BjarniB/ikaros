
#include "IKAROS.h"
#include <vector>


class Packet{
public:
	Packet( unsigned char* buf );
	~Packet();
	void ParsePacket();

	unsigned char buffer;
	char type;
	int id;
	std::vector<float>* values1;
	std::vector<float>* values2;
};