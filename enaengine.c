//Sys includes

//local includes
#include "globaldefs.h"
#include "enaengine.h"
#include "glmanager.h"
#include "sdlmanager.h"

//main

int main(int argc, char *argv[]){
	sdlInit(800, 600, 24, 1);
	if(glInit()){
		printf("opengl has initailized correctly\n");
	}
	glMainDraw();
	sleep(5);

	while(TRUE){
		//main loop
		return FALSE;
	}
	return FALSE;
}
