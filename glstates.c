#include <GL/glew.h>
#include <GL/gl.h>

#include "glstates.h"

GLenum blendsource;
GLenum blenddest;

GLenum depthfunc;

GLenum cullface;

char enabledstates = 0;

void statesBlendFunc(const GLenum source, const GLenum dest){
	if(source == blendsource && dest == blenddest)return;
	glBlendFunc(source, dest);
	blendsource = source;
	blenddest = dest;
}
void statesDepthFunc(const GLenum dfunc){
	if(dfunc == depthfunc) return;
	glDepthFunc(dfunc);
	depthfunc = dfunc;
}
void statesCullFace(const GLenum face){
	if(face == cullface) return;
	glCullFace(face);
	cullface = face;
}

void statesEnableForce(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			enabledstates = enabledstates | STATESENABLEDEPTH;
		break;
		case GL_BLEND:
			enabledstates = enabledstates | STATESENABLEBLEND;
		break;
		case GL_CULL_FACE:
			enabledstates = enabledstates | STATESENABLECULLFACE;
		break;
		case GL_MULTISAMPLE:
			enabledstates = enabledstates | STATESENABLEMULTISAMPLE;
		break;
		case GL_ALPHA_TEST:
			enabledstates = enabledstates | STATESENABLEALPHATEST;
		break;
		default:
		break;
	}
	glEnable(en);
}

void statesEnable(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			if(!(enabledstates & STATESENABLEDEPTH)){
				enabledstates = enabledstates | STATESENABLEDEPTH;
				glEnable(en);
			}
		break;
		case GL_BLEND:
			if(!(enabledstates & STATESENABLEBLEND)){
				enabledstates = enabledstates | STATESENABLEBLEND;
				glEnable(en);
			}
		break;
		case GL_CULL_FACE:
			if(!(enabledstates & STATESENABLECULLFACE)){
				enabledstates = enabledstates | STATESENABLECULLFACE;
				glEnable(en);
			}
		break;
		case GL_MULTISAMPLE:
			if(!(enabledstates & STATESENABLEMULTISAMPLE)){
				enabledstates = enabledstates | STATESENABLEMULTISAMPLE;
				glEnable(en);
			}
		break;
		case GL_ALPHA_TEST:
			if(!(enabledstates & STATESENABLEALPHATEST)){
				enabledstates = enabledstates | STATESENABLEALPHATEST;
				glEnable(en);
			}
		break;
		default:
			glEnable(en);
		break;
	}
}


void statesDisableForce(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			enabledstates = enabledstates & (~STATESENABLEDEPTH);
		break;
		case GL_BLEND:
			enabledstates = enabledstates & (~STATESENABLEBLEND);
		break;
		case GL_CULL_FACE:
			enabledstates = enabledstates & (~STATESENABLECULLFACE);
		break;
		case GL_MULTISAMPLE:
			enabledstates = enabledstates & (~STATESENABLEMULTISAMPLE);
		break;
		case GL_ALPHA_TEST:
			enabledstates = enabledstates & (~STATESENABLEALPHATEST);
		break;
		default:
		break;
	}
	glDisable(en);
}

void statesDisable(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			if(enabledstates & STATESENABLEDEPTH){
				enabledstates = enabledstates & (~STATESENABLEDEPTH); // better way of doing this?
				glDisable(en);
			}
		break;
		case GL_BLEND:
			if(enabledstates & STATESENABLEBLEND){
				enabledstates = enabledstates & (~STATESENABLEBLEND); // better way of doing this?
				glDisable(en);
			}
		break;
		case GL_CULL_FACE:
			if(enabledstates & STATESENABLECULLFACE){
				enabledstates = enabledstates & (~STATESENABLECULLFACE); // better way of doing this?
				glDisable(en);
			}
		break;
		case GL_MULTISAMPLE:
			if(enabledstates & STATESENABLEMULTISAMPLE){
				enabledstates = enabledstates & (~STATESENABLEMULTISAMPLE); // better way of doing this?
				glDisable(en);
			}
		break;
		case GL_ALPHA_TEST:
			if(enabledstates & STATESENABLEALPHATEST){
				enabledstates = enabledstates & (~STATESENABLEALPHATEST); // better way of doing this?
				glDisable(en);
			}
		break;
		default:
			glDisable(en);
		break;
	}
}
