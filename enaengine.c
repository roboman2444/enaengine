//Sys includes

//local includes
#include "globaldefs.h"
#include "enaengine.h"
#include "glmanager.h"

#include "vkmanager.h"
//#include "sdlmanager.h"
#include "glfwmanager.h"
#include "console.h"
#include "gamecodemanager.h"
#include "cvarmanager.h"
//#include "worldmanager.h"
extern int initWorldSystem(void);
extern int worldOK;


cvar_t cvar_gl_vulkan = {CVAR_SAVEABLE, "gl_vulkan", "Render using the vulkan api instead of opengl", "0"};

//main
extern double glfwGetTime(void);
//extern int SDL_GetTicks(); //will remove later todo
int main(int argc, char *argv[]){
	double to, t;
//	unsigned int to, t;
	unsigned int framecount = 0;
	cvar_init();
	if(!cvar_ok){
		//todo some sorta shutdown path
		printf("ERROR initializing cvar system!\n");
		exit(1);
	} else {
		printf("Cvar system has initialized correctly\n");
	}
	cvar_register(&cvar_gl_vulkan);
        cvar_pset(&cvar_gl_vulkan, "1");
	console_init();
//	sdlInit(800, 600, 24, 1);
	glfw_init(800, 600, 24,1);


	if(cvar_gl_vulkan.valueint){
#ifdef VULKAN_COMPILE
		if(vk_init()){
			console_printf("vulkan has initailized correctly\n");
		}
#else
		printf("Compiled without vulkan support!, reverting to GL\n");
        	cvar_pset(&cvar_gl_vulkan, "0");
		if(glInit()){
			console_printf("opengl has initailized correctly\n");
		}
#endif
	} else {
		if(glInit()){
			console_printf("opengl has initailized correctly\n");
		}
	}
	initWorldSystem();
	if(worldOK){
		console_printf("world has initailized correctly\n");
	}
	initGameCodeSystem();
	if(gamecodeOK){
		console_printf("gamecode has initailized correctly\n");
	} else {
		//todo
		console_printf("gamecode has failed to load\n");
	}
//	printConsoleBackwards();
//	to = SDL_GetTicks();
	to = glfwGetTime();


//	unsigned int timesincelastfpsupdate = 0;
//	unsigned int accum = 0;
	double timesincelastfpsupdate = 0;
	double accum = 0;
	while(TRUE){
//		t = SDL_GetTicks();
		t = glfwGetTime();
//		unsigned int delta = t-to;
		double delta = (t-to);
		to = t;
		timesincelastfpsupdate += delta;
	/*
		if(timesincelastfpsupdate > 10000){
			console_printf("%f fps\n", (float)framecount*1000.0/(float)timesincelastfpsupdate);
			timesincelastfpsupdate -= 10000;
			framecount = 0;
		}
	*/
//		if(timesincelastfpsupdate > 10000){
		if(timesincelastfpsupdate > 10.000){
//			console_printf("%f fps\n", (float)framecount*1000.0/(float)timesincelastfpsupdate);
			console_printf("%f fps\n", (double)framecount/timesincelastfpsupdate);
//			timesincelastfpsupdate -= 10000;
			timesincelastfpsupdate -=10.0;
			framecount = 0;
		}

		accum+= delta;
//		sdlCheckEvent();
		glfw_checkEvent();
#ifdef DEBUGTIMESTEP
		gameCodeTick();
		accum = 0;
#else
//		while(accum>GCTIMESTEP){
		while(accum*1000.0>GCTIMESTEP){
			gameCodeTick();
//			accum-=GCTIMESTEP;
			accum-=(double)GCTIMESTEP/1000.0;
		}
#endif

#ifdef VULKAN_COMPILE
		if(cvar_gl_vulkan.valueint) vk_mainDraw();
		else glMainDraw();
#else
	glMainDraw();
#endif
		framecount++;
	}
	return FALSE;
}
