
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
				printf("\n");
				break;
			case ' ':
				x++;
				skip = false;
				break;
			case ':':
				mat[y][x] = stof(val,NULL);
				printf("%f ", mat[y][x]);
				val = "";
				skip = true;
				break;
			default:
				if(!skip){
					val.push_back(buf[i]);
					printf("%s\n", val.c_str());
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
				printf("%f ", mat[y][x]);
				val = "";
				if(buf[i] == '#' || buf[i] == '\0'){
					y++;
					x = 0;
					printf("\n");
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
					printf("%s \n", val.c_str());
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

	const char * SetupSendData(float ** val1, float ** val2, int size_x, int size_y, int *& size){
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
		printf("%s\n", ret.c_str());
		size = (int*)ret.size();
		return ret.c_str();
	}

}; 