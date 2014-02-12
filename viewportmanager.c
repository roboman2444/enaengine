//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>
#include <tgmath.h>

//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "viewportmanager.h"
#include "framebuffermanager.h"

int vpnumber = 0; //the first is an error one/screen
int viewportsOK = 0;
viewport_t **vplist;
viewport_t * defaultViewport;

int initViewportSystem(void){
	//todo have it figure out screen aspect for the default
			//	name	id	aspect	fov	viewchanged
//	viewport_t screen = {"default"	,0 	,1.0	, 90.0, 	0};
	viewport_t screen = createViewport("default");
	if(vplist) free(vplist);
	vplist = malloc(vpnumber * sizeof(viewport_t *));
	if(!vplist) memset(vplist, 0 , vpnumber * sizeof(viewport_t *));
	defaultViewport = addViewportToList(screen);
	viewportsOK = TRUE;
	return TRUE; // todo error check
}
viewport_t *  addViewportToList(viewport_t vp){ //todo have this return a viewport pointa
	viewport_t *pointvp = malloc(sizeof(viewport_t));
	*pointvp = vp;
	int current = vpnumber;
	vpnumber++;
	vplist = realloc(vplist, vpnumber * sizeof(viewport_t*));
	vplist[current] = pointvp;
	//vplist[current].name = malloc(sizeof(*vp.name));
	//strcpy(vplist[current].name, vp.name);
	return pointvp;
}

viewport_t * createAndAddViewport(char * name){
	return addViewportToList(createViewport(name));
}

viewport_t * returnViewport(int id){
	if(id >= vpnumber) return vplist[0];
	return vplist[id];
}
viewport_t * findViewportByName(char * name){
	int i;
	for(i = 0; i<vpnumber; i++){
		if(!strcmp(name, vplist[i]->name)) return vplist[i];
	}
	return vplist[0];
}
void recalcViewMatrix(viewport_t * v){
/*	Matrix4x4_CreateIdentity(&v->view);
	Matrix4x4_ConcatRotate(&v->view, v->angle[2], 0.0, 0.0, 1.0);
	Matrix4x4_ConcatRotate(&v->view, v->angle[0], -1.0, 0.0, 0.0);
	Matrix4x4_ConcatRotate(&v->view, v->angle[1], 0.0, 1.0, 0.0);
	Matrix4x4_ConcatRotate(&v->view, -90.0, 1.0, 0.0, 0.0); // rotate up?
	Matrix4x4_ConcatScale3(&v->view, 1.0, -1.0, 1.0);
	Matrix4x4_ConcatTranslate(&v->view, -v->pos[0], -v->pos[1], -v->pos[2]);
*/
//	Matrix4x4_CreateIdentity(&v->view);
//	Matrix4x4_ConcatRotate(&v->view, -90.0, 1.0, 0.0, 0.0); // rotate up?
	Matrix4x4_CreateRotate(&v->view, v->angle[2], 0.0, 0.0, 1.0);
	Matrix4x4_ConcatRotate(&v->view, v->angle[0], 1.0, 0.0, 0.0);
	Matrix4x4_ConcatRotate(&v->view, v->angle[1], 0.0, 1.0, 0.0);
	Matrix4x4_ConcatTranslate(&v->view, -v->pos[0], -v->pos[1], -v->pos[2]);
//	Matrix4x4_ConcatScale3(&v->view, 1.0, -1.0, 1.0);

}
void recalcProjectionMatrix(viewport_t * v){
	double sine, cotangent, deltaZ;
	double radians = v->fov / 2 * M_PI / 180;

	deltaZ = v->far - v->near;
	sine = sin(radians);
	if ((deltaZ == 0) || (sine == 0) || (v->aspect == 0)) {
		return;
	}
	cotangent = cos(radians) / sine;

//	Matrix4x4_CreateIdentity(&v->projection);

	v->projection.m[0][0] = cotangent / v->aspect;
	v->projection.m[1][1] = cotangent;
	v->projection.m[2][2] = -(v->far + v->near) / deltaZ;
	v->projection.m[2][3] = -1;
	v->projection.m[3][2] = -2 * v->near * v->far / deltaZ;
	v->projection.m[3][3] = 0;

}
int recalcViewport(viewport_t * v, vec3_t pos, vec3_t angle, float fov, float aspect, float near, float far){
	if(pos != v->pos || angle != v->angle){
		v->viewchanged = TRUE;
		v->pos[0] = pos[0];
		v->pos[1] = pos[1];
		v->pos[2] = pos[2];
		v->angle[0] = angle[0];
		v->angle[1] = angle[1];
		v->angle[2] = angle[2];
		recalcViewMatrix(v);
	}
	if(fov != v->fov || aspect != v->aspect || v->near!= near || v->far != far){
		v->viewchanged = TRUE;
		v->aspect = aspect;
		v->fov = fov;
		recalcProjectionMatrix(v);
	}
//	if(v->viewchanged) Matrix4x4_Concat(&v->viewproj, &v->view, &v->projection);
	if(v->viewchanged) Matrix4x4_Concat(&v->viewproj, &v->projection, &v->view);
	return v->viewchanged;
}
viewport_t createViewport (char * name){
	viewport_t v;
	v.id = 0; //todo make useful
	v.aspect = 1.0;
	v.fov = 90.0;
	v.near = 1.0;
	v.far = 1000.0;
	v.viewchanged = TRUE;
	int i;
	for(i = 0; i < 3; i++){
		v.pos[i] = 0.0;
		v.angle[i] = 0.0;
	}
 	v.name = malloc(strlen(name)+1);
	strcpy(v.name, name);
//	recalcViewMatrix(&v); //todo may not need
//	recalcProjectionMatrix(&v);
	Matrix4x4_CreateIdentity(&v.view);
	Matrix4x4_CreateIdentity(&v.projection);
	Matrix4x4_CreateIdentity(&v.viewproj);
	return v;
//todo
}
