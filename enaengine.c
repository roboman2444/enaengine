//Sys includes

//local includes
#include "globaldefs.h"
#include "enaengine.h"
#include "glmanager.h"
#include "sdlmanager.h"
#include "console.h"
#include "gamecodemanager.h"

//main

extern int SDL_GetTicks(); //will remove later todo
int main(int argc, char *argv[]){
	unsigned int to, t;
	unsigned int framecount = 0;
	initConsoleSystem();
	sdlInit(800, 600, 24, 1);
	if(glInit()){
		consolePrintf("opengl has initailized correctly\n");
	}
	initGameCodeSystem();
	if(gamecodeOK){
		consolePrintf("gamecode has initailized correctly\n");
	}
//	printConsoleBackwards();
	to = SDL_GetTicks();

	unsigned int timesincelastfpsupdate = 0;
	unsigned int accum = 0;
	while(TRUE){//glorious for loop master race
		t = SDL_GetTicks();
		unsigned int delta = t-to;
		to = t;
		timesincelastfpsupdate += delta;
	/*
		if(timesincelastfpsupdate > 10000){
			consolePrintf("%f fps\n", (float)framecount*1000.0/(float)timesincelastfpsupdate);
			timesincelastfpsupdate -= 10000;
			framecount = 0;
		}
	*/
		if(timesincelastfpsupdate > 10000){
			consolePrintf("%f fps\n", (float)framecount*1000.0/(float)timesincelastfpsupdate);
			timesincelastfpsupdate -= 10000;
			framecount = 0;
		}

		accum+= delta;
		sdlCheckEvent();
		while(accum>GCTIMESTEP){
			gameCodeTick();
			accum-=GCTIMESTEP;
		}
		glMainDraw();
		framecount++;
	}
	return FALSE;
}
