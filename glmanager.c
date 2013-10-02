#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "glmanager.h"


int glInit(void){
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("ERROR with the glew: %s\n", glewGetErrorString(glewError));
		return FALSE;
	}
	return TRUE; // so far so good
}
