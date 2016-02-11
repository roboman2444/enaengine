//global includes
#include <GL/glew.h>
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "mathlib.h"
#include "viewportmanager.h"
#include "shadermanager.h"
#include "renderqueue.h"
#include "glstates.h"
#include "glmanager.h"

#include "drawbb.h"


int drawbb_ok = 0;
unsigned int drawbbshaderid;

int drawbb_init(void){
	drawbbshaderid = shader_createAndAddRINT("wireframe");
	drawbb_ok = TRUE;
	return TRUE;
}
void drawbb_drawCallback(renderlistitem_t *ilist, unsigned int count){
}

unsigned int drawbb_addToRenderQueue(viewport_t *v, renderqueue_t *q, const vec_t *bboxp){
	return FALSE;
}
