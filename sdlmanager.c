#include "SDL.h"

#include "globaldefs.h"
#include "sdlmanager.h"
#include "console.h"

//local vars
SDL_Surface *surface;
const SDL_VideoInfo *videoInfo;
int videoFlags;

//functions
int resizeWindow(int width, int height, int bpp, int debugmode){
	if(debugmode) consolePrintf("DEBUG -- SDL video resize to: %ix%i with %i bits per pixel \n", width, height, bpp);
	if(height<1) height = 1;
	if(width<1) width = 1;
	surface = SDL_SetVideoMode(width, height, bpp, videoFlags);
	if(!surface){
//		fprintf(stderr, "ERROR -- SDL video resize failed: %s \n", SDL_GetError());
		consolePrintf("ERROR -- SDL video resize failed: %s \n", SDL_GetError());
		return FALSE;
	}
	return TRUE;
}
int setMSAA(int samples){
	int msbuf;
	int mssamp;
	if(!samples){
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	}
	else{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
//		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &msbuf);//todo
	}
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
	setMSAA(16);
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
				SDL_Quit();
			break;
			case SDL_KEYDOWN:
				//todo
			break;
			case SDL_KEYUP:
				//todo
			break;
			case SDL_VIDEORESIZE:
				resizeWindow(event.resize.w, event.resize.h, 24, 0);
			break;
			case SDL_MOUSEMOTION:
			//todo
			break;
			case SDL_MOUSEBUTTONDOWN:
			//todo
			break;
			case SDL_MOUSEBUTTONUP:
			break;
			default:
			break;
		}
	}
	return TRUE;
}
