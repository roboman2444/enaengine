#include <GL/glew.h>
#include <GL/gl.h>

#include "glstates.h"



char glcurrentstate = 0;
char glcurrentactivetexture = 0; //todo

void glStatesSetFlag(char flag){
	if((glcurrentstate ^ flag) & FLAG_BLEND){
		if(flag & FLAG_BLEND){
			glEnable(GL_BLEND);
		} else {
			glDisable(GL_BLEND);
		}
	}
	if((glcurrentstate ^ flag) & FLAG_DEPTHTEST){
		if(flag & FLAG_BLEND){
			glEnable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_DEPTH_TEST);
		}
	}
	if((glcurrentstate ^ flag) & FLAG_DEPTHMASK){
		if(flag & FLAG_BLEND){
			glDepthMask(GL_TRUE);
		} else {
			glDepthMask(GL_FALSE);
		}
	}
	if((glcurrentstate ^ flag) & FLAG_CULLFACE){
		if(flag & FLAG_BLEND){
			glEnable(GL_CULL_FACE);
		} else {
			glDisable(GL_CULL_FACE);
		}
	}
	if((glcurrentstate ^ flag) & FLAG_CULLSIDE){
		if(flag & FLAG_BLEND){
			glCullFace(GL_FRONT);
		} else {
			glCullFace(GL_BACK);
		}
	}
}
