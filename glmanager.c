#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "glmanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "vbomanager.h"
#include "modelmanager.h"


int texturesOK;
int framebuffersOK;
int modelsOK;
int vboOK;

//todo move these to globals in their respective .hfiles

int glInit(void){
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("ERROR with the glew: %s\n", glewGetErrorString(glewError));
		return FALSE;
	}
	texturesOK = initTextureSystem();
	if(!texturesOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	framebuffersOK = initFrameBufferSystem();
	if(!framebuffersOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	vboOK = initVBOSystem();
	if(!vboOK){
		return FALSE;
		//todo call some sort of shutdown of everything
	}
	modelsOK = initModelSystem();
	if(!modelsOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	createAndAddModel("tehball");

	return TRUE; // so far so good
}
