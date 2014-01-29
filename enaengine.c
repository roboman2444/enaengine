//Sys includes

//local includes
#include "globaldefs.h"
#include "enaengine.h"
#include "glmanager.h"
#include "sdlmanager.h"
#include "console.h"

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
//	printConsoleBackwards();
	to = SDL_GetTicks();
	while(TRUE){
		glMainDraw();
		framecount++;
		if(framecount == 1000){
			t = SDL_GetTicks();
			consolePrintf("%f fps\n", 1000000.0f/(t-to));
			to = t;
			framecount = 0;
		}
		//main loop
	//	return FALSE;
	}
	return FALSE;
}
