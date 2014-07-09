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



void statesEnable(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			if(!(enabledstates & STATESENABLEDEPTH)) glEnable(en);
		break;
		case GL_BLEND:
			if(!(enabledstates & STATESENABLEBLEND)) glEnable(en);
		break;
		case GL_CULL_FACE:
			if(!(enabledstates & STATESENABLECULLFACE)) glEnable(en);
		break;
		case GL_MULTISAMPLE:
			if(!(enabledstates & STATESENABLEMULTISAMPLE)) glEnable(en);
		break;
		case GL_ALPHA_TEST:
			if(!(enabledstates & STATESENABLEALPHATEST)) glEnable(en);
		break;
		default:
			glEnable(en);
		break;
	}
}
void statesDisable(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			if(enabledstates & STATESENABLEDEPTH) glDisable(en);
		break;
		case GL_BLEND:
			if(enabledstates & STATESENABLEBLEND) glDisable(en);
		break;
		case GL_CULL_FACE:
			if(enabledstates & STATESENABLECULLFACE) glDisable(en);
		break;
		case GL_MULTISAMPLE:
			if(enabledstates & STATESENABLEMULTISAMPLE) glDisable(en);
		break;
		case GL_ALPHA_TEST:
			if(enabledstates & STATESENABLEALPHATEST) glDisable(en);
		break;
		default:
			glDisable(en);
		break;
	}
}
