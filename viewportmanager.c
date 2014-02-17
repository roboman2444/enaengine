//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>
#include <tgmath.h>

//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "hashtables.h"
#include "viewportmanager.h"
#include "framebuffermanager.h"

int viewportsOK = 0;
int viewportcount = 0;
int viewportArrayFirstOpen = 0;
int viewportArrayLastTaken = 0;
int viewportArraySize = 0;
viewport_t *viewportlist;

hashbucket_t viewporthashtable[MAXHASHBUCKETS];

int initViewportSystem(void){
//	viewport_t screen = createViewport("default");
	memset(viewporthashtable, 0, MAXHASHBUCKETS*sizeof(hashbucket_t));
	if(viewportlist) free(viewportlist);
	viewportlist = 0;
//	viewportlist = malloc(vpnumber * sizeof(viewport_t));
//	if(!viewportlist) memset(viewportlist, 0 , vpnumber * sizeof(viewport_t));
//	defaultViewport = addViewportToList(screen);
	viewportsOK = TRUE;
	return TRUE; // todo error check
}
viewportlistpoint_t findViewportssByNameRPOINT(char * name){
	viewportlistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &viewporthashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
//			return returnById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(viewport_t *));
			ret.list[ret.count-1] = returnViewportById(hb->id);
		}
        }
	return ret;
}
viewportlistint_t findViewportssByNameRINT(char * name){
	viewportlistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &viewporthashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
//			return returnById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
        }
	return ret;
}
viewport_t *findViewportByNameRPOINT(char * name){
	return returnViewportById(findByNameRINT(name, viewporthashtable));
}
int findViewportByNameRINT(char * name){
	return findByNameRINT(name, viewporthashtable);
}
int deleteViewPort(int id){
	int viewportindex = (id & 0xFFFF);
	viewport_t * viewport = &viewportlist[viewportindex];
	if(viewport->myid != id) return FALSE;
	if(!viewport->name) return FALSE;
	deleteFromHashTable(viewport->name, id, viewporthashtable);
	free(viewport->name);

//todo free viewport
	bzero(viewport, sizeof(viewport_t));
	if(viewportindex < viewportArrayFirstOpen) viewportArrayFirstOpen = viewportindex;
	for(; viewportArrayLastTaken > 0 && !viewportlist[viewportArrayLastTaken].type; viewportArrayLastTaken--);
	return TRUE;
}

viewport_t * returnViewportById(int id){
	int viewportindex = (id & 0xFFFF);
	viewport_t * viewport = &viewportlist[viewportindex];
	if(!viewport->type) return FALSE;
	if(viewport->myid == id) return viewport;
	return FALSE;
}

viewport_t createViewport (char * name, char type){
	viewport_t v;
	v.type = 0; //todo make useful
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
	v.type = type;
	return v;
//todo
}

int addViewportRINT(viewport_t viewport){
	viewportcount++;
	for(; viewportArrayFirstOpen < viewportArraySize && viewportlist[viewportArrayFirstOpen].type; viewportArrayFirstOpen++);
	if(viewportArrayFirstOpen == viewportArraySize){	//resize
		viewportArraySize++;
		viewportlist = realloc(viewportlist, viewportArraySize * sizeof(viewport_t));
	}
	viewportlist[viewportArrayFirstOpen] = viewport;
	int returnid = (viewportcount << 16) | viewportArrayFirstOpen;
	viewportlist[viewportArrayFirstOpen].myid = returnid;

	addToHashTable(viewportlist[viewportArrayFirstOpen].name, returnid, viewporthashtable);
	if(viewportArrayLastTaken < viewportArrayFirstOpen) viewportArrayLastTaken = viewportArrayFirstOpen; //todo redo
	return returnid;
}
viewport_t * addViewportRPOINT(viewport_t viewport){
	viewportcount++;
	for(; viewportArrayFirstOpen < viewportArraySize && viewportlist[viewportArrayFirstOpen].type; viewportArrayFirstOpen++);
	if(viewportArrayFirstOpen == viewportArraySize){	//resize
		viewportArraySize++;
		viewportlist = realloc(viewportlist, viewportArraySize * sizeof(viewport_t));
	}
	viewportlist[viewportArrayFirstOpen] = viewport;
	int returnid = (viewportcount << 16) | viewportArrayFirstOpen;
	viewportlist[viewportArrayFirstOpen].myid = returnid;

	addToHashTable(viewportlist[viewportArrayFirstOpen].name, returnid, viewporthashtable);
	if(viewportArrayLastTaken < viewportArrayFirstOpen) viewportArrayLastTaken = viewportArrayFirstOpen; //todo redo
	return &viewportlist[viewportArrayFirstOpen];
}

viewport_t * createAndAddViewportRPOINT(char * name, char type){
	return addViewportRPOINT(createViewport(name, type));
}
int createAndAddViewportRINT(char * name, char type){
	return addViewportRINT(createViewport(name, type));
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
