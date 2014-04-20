#include "SDL.h"

#include "globaldefs.h"
#include "sdlmanager.h"
#include "console.h"

//local vars
SDL_Surface *surface;
const SDL_VideoInfo *videoInfo;
int videoFlags;

//functions
extern int glResizeViewports(int width, int height);
int resizeWindow(int width, int height, int bpp, int debugmode){
	if(debugmode) consolePrintf("DEBUG -- SDL video resize to: %ix%i with %i bits per pixel \n", width, height, bpp);
	if(height<1) height = 1;
	if(width<1) width = 1;
	surface = SDL_SetVideoMode(width, height, bpp, videoFlags);
	//todo resize any framebuffers
	if(!surface){
//		fprintf(stderr, "ERROR -- SDL video resize failed: %s \n", SDL_GetError());
		consolePrintf("ERROR -- SDL video resize failed: %s \n", SDL_GetError());
		return FALSE;
	}
	int r = glResizeViewports(width, height);
	if(!r){
		consolePrintf("ERROR -- GL framebuffers resize failed\n");
		return FALSE;

	}
	return TRUE;
}
int setMSAA(int samples){
//	int msbuf = 0;
//	int mssamp = 0;
//	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &msbuf);
//	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &mssamp);
	if(samples<2){
		/*if(msbuf)*/ SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
//		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &msbuf);
//		if(msbuf); //todo cast error
	}
	else{
		/*if(!msbuf)*/ SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &msbuf);
		//if(!msbuf); //todo cast error

		//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &msbuf);//todo
		//returns 0 on success -error code on fail
	}
	/*if(mssamp != samples)*/ SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
	//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &mssamp);
	//if(mssamp != samples){
	//	return FALSE;
	//	if(samples > 1){
	//		return setMSAA(samples/2);
	//	} else return FALSE;
	//}

	return samples;
}
int sdlInit(int width, int height, int bpp, int debugmode){
	if(debugmode) consolePrintf("DEBUG -- SDL window initializing \n");
	if(SDL_Init(SDL_INIT_VIDEO)<0){
//		fprintf(stderr, "ERROR -- SDL video init failed: %s \n", SDL_GetError());
		consolePrintf("ERROR -- SDL video init failed: %s \n", SDL_GetError());
		return FALSE;
	}
	videoInfo = SDL_GetVideoInfo();
	videoFlags = SDL_OPENGL;
	videoFlags |= SDL_GL_DOUBLEBUFFER;
	videoFlags |= SDL_HWPALETTE;
	videoFlags |= SDL_RESIZABLE;
	if(videoInfo->hw_available) videoFlags |= SDL_HWSURFACE;
	else			    videoFlags |= SDL_SWSURFACE;
	if(videoInfo->blit_hw) 	    videoFlags |= SDL_HWACCEL;
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//	setMSAA(16);
	return resizeWindow(width, height, bpp, debugmode);
}
void swapBuffers(void){
	SDL_GL_SwapBuffers();
}
int sdlCheckEvent(void){
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_QUIT:
				//todo make exit functions
				SDL_Quit(); //this segfaults for some reason
//				exit(0);
			break;
			case SDL_KEYDOWN:
				//todo
			break;
			case SDL_KEYUP:
				//todo
			break;
			case SDL_VIDEORESIZE:
				resizeWindow(event.resize.w, event.resize.h, 24, 0);
				//call stuff to change stuff blah
			break;
			case SDL_MOUSEMOTION:
				//todo
			break;
			case SDL_MOUSEBUTTONDOWN:
				//todo
			break;
			case SDL_MOUSEBUTTONUP:
				//todo
			break;
			default:
			break;
		}
	}
	return TRUE;
}
