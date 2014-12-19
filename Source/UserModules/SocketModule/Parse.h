
#include "IKAROS.h"
#include <vector>
#include <string>
#include <sstream>

namespace parse{

	float ** ParseValue1(char * buf, int buf_size, int sizeY){
		float** mat = create_matrix(buf_size,sizeY);
		int x = 0;
		int y = 0;
		int i = 0;
		bool skip = false;
		std::string val = "";
		while(y < sizeY){
			switch(buf[i]){
			case '\0':
			case '#':
				mat[y][x+1] = -1;
				y++;
				x = 0;
				skip = false;
				break;
			case ' ':
				x++;
				skip = false;
				break;
			case ':':
				mat[y][x] = stof(val,NULL);
				val = "";
				skip = true;
				break;
			default:
				if(!skip){
					val.push_back(buf[i]);
				}
				break;
			}
			i++;
		}
		return mat;
	}

	float ** ParseValue2(char * buf, int buf_size, int sizeY){
		float** mat = create_matrix(buf_size,sizeY);
		int x = 0;
		int y = 0;
		int i = 0;
		bool skip = true;
		std::string val = "";
		while(y < sizeY){
			switch(buf[i]){
			case '\0':
			case '#':
				mat[y][x+1] = -1;
			case ' ':
				mat[y][x] = stof(val,NULL);
				val = "";
				if(buf[i] == '#' || buf[i] == '\0'){
					y++;
					x = 0;
				}else{
					x++;
				}
				skip = true;
				break;
			case ':':
				skip = false;
				break;
			default:
				if(!skip){
					val.push_back(buf[i]);
				}
				break;
			}
			i++;
		}

		return mat;
	}

	std::string Convert (float number){
    	std::ostringstream buff;
    	buff<<number;
    	return buff.str();   
	}

	std::string SetupSendData(float ** val1, float ** val2, int size_x, int size_y){
		std::string ret = "";
		for(int y = 0; y < size_y; ++y){
			for(int x = 0; x < size_x; x++){
				if(val1[y][x] == -1){
					if(y == size_y-1){
						ret += "\0";
					}else{
						ret += "#";
					}
					break;			
				}else{
					ret += 	Convert(val1[y][x]) + ":" + Convert(val2[y][x]);
					if(val1[y][x+1] != -1)
						ret += " ";
				}
			}
		}
		return ret;
	}

	// UNUSED something is wrong with this implementation, oversized x is used instead
	int CalcSizeX (char * buf, int size){
		int ret = 0;

		for(int i = 0; i < size; ++i){
			if(buf[i] == ':'){
				ret++;
			}
		}
		return ret;
	}

	float ParseFlag(char * buf, int size){
		std::string val = "";

		for(int i = 0; i < size; ++i){
			val += buf[i];
		}


		return stof(val,NULL);
	}

	float ParsePlayTick(char * buf, int size){
		std::string val = "";

		for(int i = 1; i < size; ++i){
			val += buf[i];
		}

		return stof(val,NULL);
	}
}; 