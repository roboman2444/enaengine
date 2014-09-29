#include <GL/glew.h>
#include <GL/gl.h>

#include "glstates.h"

glstate_t state;

void states_blendFunc(const GLenum source, const GLenum dest){
	if(source == state.blendsource && dest == state.blenddest)return;
	glBlendFunc(source, dest);
	state.blendsource = source;
	state.blenddest = dest;
}
void states_alpha(const GLenum func, const GLclampf ref){
	if(func == state.alphafunc && ref == state.alpharef)return;
	glAlphaFunc(func, ref);
	state.alphafunc = func;
	state.alpharef = ref;
}
void states_depthFunc(const GLenum dfunc){
	if(dfunc == state.depthfunc) return;
	glDepthFunc(dfunc);
	state.depthfunc = dfunc;
}
void states_cullFace(const GLenum face){
	if(face == state.cullface) return;
	glCullFace(face);
	state.cullface = face;
}
void states_depthMask(const GLenum mask){
	if(mask == state.depthmask) return;
	glDepthMask(mask);
	state.depthmask = mask;
}
void states_bindVertexArray(const GLuint id){
	if(id != state.vaoid){
		glBindVertexArray(id);
		state.vaoid = id;
	}
}
void states_bindBuffer(const GLenum type, const GLuint id){
	if(type != state.vbotype || state.vborangei ||id != state.vboid || state.vborangeo || state.vboranges){
		glBindBuffer(type, id);
		state.vbotype = type;
		state.vborangei = 0;
		state.vboid = id;
		state.vborangeo = 0;
		state.vboranges = 0;
	}
}
void states_bindBufferRange(const GLenum type, const GLuint index, const GLuint id, const GLintptr offset, const GLsizeiptr size){
	if(type != state.vbotype || index != state.vborangei||id != state.vboid || offset != state.vborangeo || size != state.vboranges){
		glBindBufferRange(type, index, id, offset, size);
		state.vbotype = type;
		state.vborangei = index;
		state.vboid = id;
		state.vborangeo = offset;
		state.vboranges = size;
	}
}



void states_enableForce(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			state.enabledstates = state.enabledstates | STATESENABLEDEPTH;
		break;
		case GL_BLEND:
			state.enabledstates = state.enabledstates | STATESENABLEBLEND;
		break;
		case GL_CULL_FACE:
			state.enabledstates = state.enabledstates | STATESENABLECULLFACE;
		break;
		case GL_MULTISAMPLE:
			state.enabledstates = state.enabledstates | STATESENABLEMULTISAMPLE;
		break;
		case GL_ALPHA_TEST:
			state.enabledstates = state.enabledstates | STATESENABLEALPHATEST;
		break;
		default:
		break;
	}
	glEnable(en);
}

void states_enable(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			if(!(state.enabledstates & STATESENABLEDEPTH)){
				state.enabledstates = state.enabledstates | STATESENABLEDEPTH;
				glEnable(en);
			}
		break;
		case GL_BLEND:
			if(!(state.enabledstates & STATESENABLEBLEND)){
				state.enabledstates = state.enabledstates | STATESENABLEBLEND;
				glEnable(en);
			}
		break;
		case GL_CULL_FACE:
			if(!(state.enabledstates & STATESENABLECULLFACE)){
				state.enabledstates = state.enabledstates | STATESENABLECULLFACE;
				glEnable(en);
			}
		break;
		case GL_MULTISAMPLE:
			if(!(state.enabledstates & STATESENABLEMULTISAMPLE)){
				state.enabledstates = state.enabledstates | STATESENABLEMULTISAMPLE;
				glEnable(en);
			}
		break;
		case GL_ALPHA_TEST:
			if(!(state.enabledstates & STATESENABLEALPHATEST)){
				state.enabledstates = state.enabledstates | STATESENABLEALPHATEST;
				glEnable(en);
			}
		break;
		default:
			glEnable(en);
		break;
	}
}


void states_disableForce(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			state.enabledstates = state.enabledstates & (~STATESENABLEDEPTH);
		break;
		case GL_BLEND:
			state.enabledstates = state.enabledstates & (~STATESENABLEBLEND);
		break;
		case GL_CULL_FACE:
			state.enabledstates = state.enabledstates & (~STATESENABLECULLFACE);
		break;
		case GL_MULTISAMPLE:
			state.enabledstates = state.enabledstates & (~STATESENABLEMULTISAMPLE);
		break;
		case GL_ALPHA_TEST:
			state.enabledstates = state.enabledstates & (~STATESENABLEALPHATEST);
		break;
		default:
		break;
	}
	glDisable(en);
}

void states_disable(GLenum en){
	switch(en){
		case GL_DEPTH_TEST:
			if(state.enabledstates & STATESENABLEDEPTH){
				state.enabledstates = state.enabledstates & (~STATESENABLEDEPTH); // better way of doing this?
				glDisable(en);
			}
		break;
		case GL_BLEND:
			if(state.enabledstates & STATESENABLEBLEND){
				state.enabledstates = state.enabledstates & (~STATESENABLEBLEND); // better way of doing this?
				glDisable(en);
			}
		break;
		case GL_CULL_FACE:
			if(state.enabledstates & STATESENABLECULLFACE){
				state.enabledstates = state.enabledstates & (~STATESENABLECULLFACE); // better way of doing this?
				glDisable(en);
			}
		break;
		case GL_MULTISAMPLE:
			if(state.enabledstates & STATESENABLEMULTISAMPLE){
				state.enabledstates = state.enabledstates & (~STATESENABLEMULTISAMPLE); // better way of doing this?
				glDisable(en);
			}
		break;
		case GL_ALPHA_TEST:
			if(state.enabledstates & STATESENABLEALPHATEST){
				state.enabledstates = state.enabledstates & (~STATESENABLEALPHATEST); // better way of doing this?
				glDisable(en);
			}
		break;
		default:
			glDisable(en);
		break;
	}
}

