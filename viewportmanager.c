//global includes
#include <GL/glew.h>
#include <GL/gl.h>
#include <tgmath.h>

//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "hashtables.h"
#include "viewportmanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "mathlib.h"
int viewport_ok = 0;
int viewportcount = 0;
int viewportArrayFirstOpen = 0;
int viewportArrayLastTaken = -1;
int viewportArraySize = 0;
viewport_t *viewportlist;

hashbucket_t viewporthashtable[MAXHASHBUCKETS];

int viewport_init(void){
//	viewport_t screen = createViewport("default");
	memset(viewporthashtable, 0, MAXHASHBUCKETS*sizeof(hashbucket_t));
	if(viewportlist) free(viewportlist);
	viewportlist = 0;
//	viewportlist = malloc(vpnumber * sizeof(viewport_t));
//	if(!viewportlist) memset(viewportlist, 0 , vpnumber * sizeof(viewport_t));
//	defaultViewport = addViewportToList(screen);
	viewport_ok = TRUE;
	return TRUE; // todo error check
}
viewportlistpoint_t findViewportsByNameRPOINT(char * name){
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
viewportlistint_t findViewportsByNameRINT(char * name){
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
viewport_t * findViewportByNameRPOINT(char * name){
	return returnViewportById(findByNameRINT(name, viewporthashtable));
}
int findViewportByNameRINT(char * name){
	return findByNameRINT(name, viewporthashtable);
}
int deleteViewport(const int id){
	int viewportindex = (id & 0xFFFF);
	viewport_t * viewport = &viewportlist[viewportindex];
	if(viewport->myid != id) return FALSE;
	if(!viewport->name) return FALSE;
	deleteFromHashTable(viewport->name, id, viewporthashtable);
	free(viewport->name);

//todo free viewport
	memset(viewport, 0, sizeof(viewport_t));
	if(viewportindex < viewportArrayFirstOpen) viewportArrayFirstOpen = viewportindex;
	for(; viewportArrayLastTaken > 0 && !viewportlist[viewportArrayLastTaken].type; viewportArrayLastTaken--);
	return TRUE;
}

viewport_t * returnViewportById(const int id){
	int index = (id & 0xFFFF);
//	if(index > viewportArrayLastTaken) return FALSE;
	viewport_t * v = &viewportlist[index];
	if(!v->type) return FALSE;
	if(v->myid == id) return v;
	return FALSE;
}

viewport_t createViewport (char * name, const char type){
	viewport_t v;
	v.type = 0; //todo make useful
	v.aspect = 1.0f;
	v.fov = 90.0f;
	v.near = 1.0f;
	v.far = 1000.0f;
	v.viewchanged = TRUE;
	v.outfbid = 0;
	v.dfbid = 0;
	int i;
	for(i = 0; i < 3; i++){
		v.pos[i] = 0.0f;
		v.angle[i] = 0.0f;
	}
 	v.name = malloc(strlen(name)+1);
	strcpy(v.name, name);
	Matrix4x4_CreateIdentity(&v.view);
	Matrix4x4_CreateIdentity(&v.cam);
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
void pruneViewportList(void){
	if(viewportArraySize == viewportArrayLastTaken+1) return;
	viewportArraySize = viewportArrayLastTaken+1;
	viewportlist = realloc(viewportlist, viewportArraySize * sizeof(viewport_t));
}


viewport_t * createAndAddViewportRPOINT(char * name, char type){
	return addViewportRPOINT(createViewport(name, type));
}
int createAndAddViewportRINT(char * name, char type){
	return addViewportRINT(createViewport(name, type));
}


vec3_t stockv_forward = { 0.0, 0.0, 1.0 };
vec3_t stockv_up = { 0.0, 1.0, 0.0 };
vec3_t stockv_right = { 1.0, 0.0, 0.0 };


void recalcViewMatrix(viewport_t * v){
	Matrix4x4_CreateRotate(&v->view, v->angle[2], 0.0f, 0.0f, 1.0f);
	Matrix4x4_ConcatRotate(&v->view, v->angle[0], 1.0f, 0.0f, 0.0f);
	Matrix4x4_ConcatRotate(&v->view, v->angle[1], 0.0f, 1.0f, 0.0f);
	//putting in vectors stuff here, because why not?
	Matrix4x4_Transform(&v->view, stockv_forward, v->v_forward);
	Matrix4x4_Transform(&v->view, stockv_up, v->v_up);
	Matrix4x4_Transform(&v->view, stockv_right, v->v_right);
	Matrix4x4_ConcatTranslate(&v->view, -v->pos[0], -v->pos[1], -v->pos[2]);
}
void recalcProjectionMatrix(viewport_t * v){
	double sine, cotangent, deltaZ;
	double radians = v->fov / 2.0 * M_PI / 180.0;

	deltaZ = v->far - v->near;
	sine = sin(radians);
	if ((deltaZ == 0) || (sine == 0) || (v->aspect == 0)) {
		return;
	}
	cotangent = cos(radians) / sine;
//	const float nudge = 1.0 - 1.0 / (1<<23);

	v->projection.m[0][0] = cotangent / v->aspect;
	v->projection.m[1][1] = cotangent;
	v->projection.m[2][2] = -(v->far + v->near) / deltaZ;
//	v->projection.m[2][2] = -nudge;
	v->projection.m[2][3] = -1.0;
	v->projection.m[3][2] = -2.0 * v->near * v->far / deltaZ;
//	v->projection.m[3][2] = -2 * v->near * nudge;
	v->projection.m[3][3] = 0;

}
vec_t distPointFromNear(viewport_t *v, vec_t * p){
	vec_t * n = v->frustum[5].norm;
	vec_t dist = vec3dot(n, p) + v->frustum[5].d;
	return dist;
}
int testPointInFrustum(viewport_t * v, vec_t * p){
	int i;
	vec_t * n;
	for(i = 0; i < 6; i++){
		n = v->frustum[i].norm;
		float dist = vec3dot(n, p) + v->frustum[i].d;
		if(dist < 0.0){
			return FALSE;
		}
	}
	return TRUE;
}
int testSphereInFrustum(viewport_t * v, vec_t * p, float size){
	int i;
	vec_t * n;
	for(i = 0; i < 6; i++){
		n = v->frustum[i].norm;
		float dist = vec3dot(n, p) + v->frustum[i].d;
		if(dist < -size){
			return FALSE;
		}
	}
	return TRUE;
}
int testBBoxPInFrustum(viewport_t * v, vec_t * points){
	int i;
	vec_t * n;
	float d;
	for(i = 0; i < 6; i++){
		n = v->frustum[i].norm;
		d = v->frustum[i].d;
		int j;
		for(j = 0; j < 8; j++){
			vec_t * p = &points[j*3];
			float dist = vec3dot(n, p) + d;
//			console_printf("dist:%f\n",dist);
			if(dist > 0.0) break; // point infront of the plane
		}
		if(j==8) return FALSE; //all the points failed the frustum
	}


	return TRUE;
}
int testBBoxPInFrustumCheckWhole(viewport_t * v, vec_t * points){
	int i;
	vec_t * n;
	float d;
	int mode = 0;
	for(i = 0; i < 6; i++){
		n = v->frustum[i].norm;
		d = v->frustum[i].d;
		int j;
		if(mode){
			for(j = 0; j < 8; j++){
				vec_t * p = &points[j*3];
				float dist = vec3dot(n, p) + d;
				if(dist > 0.0) break; // point infront of the plane
			}
			if(j==8) return FALSE; //all the points failed the frustum

		} else {
			int correct = 0;
			for(j = 0; j < 8; j++){
				vec_t * p = &points[j*3];
				float dist = vec3dot(n, p) + d;
				if(dist > 0.0) correct++;
			}
			if(!correct) return FALSE; // all points failed plane
			if(correct !=8 ) mode = 1; //one point didnt make plane, switch to normal mode
		}
	}
	if(mode) return TRUE;

	return 2;
}
int testSphereInFrustumNearPlane(viewport_t * v, vec_t * p, float size){
	int i;
	vec_t * n;
	for(i = 0; i < 5; i++){
		n = v->frustum[i].norm;
		float dist = vec3dot(n, p) + v->frustum[i].d;
		if(dist < -size){
			return FALSE;
		}
	}
	n = v->frustum[5].norm;
	float dist = vec3dot(n, p) + v->frustum[i].d;
	if(dist < -size){
		return FALSE;
	} else if(dist < size){
		return 2;
	}

	return TRUE;
}
int testBBoxPInFrustumNearPlane(viewport_t * v, vec_t * points){
	int i;
	vec_t * n;
	float d;
	for(i = 0; i < 5; i++){
		n = v->frustum[i].norm;
		d = v->frustum[i].d;
		int j;
		for(j = 0; j < 8; j++){
			vec_t * p = &points[j*3];
			float dist = vec3dot(n, p) + d;
//			console_printf("dist:%f\n",dist);
			if(dist > 0.0) break; // point infront of the plane
		}
		if(j==8) return FALSE; //all the points failed the frustum
	}
	n = v->frustum[5].norm;
	d = v->frustum[5].d;
	int j;
	char test = 0;
	for(j = 0; j < 8; j++){
		vec_t * p = &points[j*3];
		float dist = vec3dot(n, p) + d;
//		console_printf("dist:%f\n",dist);
		//if(dist > 0.0) break; // point infront of the plane
		//point in front of nearplane
		if(dist > 0.0){
			//pre check to see if there are points that are on both sides of plane
			if(test &2) return 2;
			test = test | 1;
		} else {
			//pre check to see if there are points on both sides of plane
			if(test & 1) return 2;
			test = test | 2;
		}
	}
	//point still behind plane
	if(test == 2) return FALSE;
	//points on both side of plane
	else if (test == 3) return 2;
	//points only in front of plane
	return TRUE;
}
void recalcFrustumBBoxP(viewport_t *v){
	float cotangent, cotaspect;
	cotangent = v->projection.m[1][1] /2;
//	cotaspect = cotangent / v->aspect;
	cotaspect = v->projection.m[0][0] /2;
	float near = v->near;
	float far = v->far;

	float nh = near * cotangent;
	float nw = near * cotaspect;
	float fh = far * cotangent;
	float fw = far * cotaspect;
	vec_t prebboxp[24];
	prebboxp[0] = -nw;
	prebboxp[1] = -nh;
	prebboxp[2] = near;
	prebboxp[3] = nw;
	prebboxp[4] = -nh;
	prebboxp[5] = near;
	prebboxp[6] = nw;
	prebboxp[7] = nh;
	prebboxp[8] = near;
	prebboxp[9] = -nw;
	prebboxp[10] = nh;
	prebboxp[11] = near;

	prebboxp[12] = -fw;
	prebboxp[13] = -fh;
	prebboxp[14] = far;
	prebboxp[15] = fw;
	prebboxp[16] = -fh;
	prebboxp[17] = far;
	prebboxp[18] = fw;
	prebboxp[19] = fh;
	prebboxp[20] = far;
	prebboxp[21] = -fw;
	prebboxp[22] = fh;
	prebboxp[23] = far;
	int i;
	for(i = 0; i < 8; i++){
		Matrix4x4_Transform(&v->cam, &prebboxp[i*3], &v->bboxp[i*3]);
	}
//or
//transform a unit cube by the inverse VP matrix
}
void recalcFrustum(viewport_t * v){
	vec_t m[16];
	Matrix4x4_ToArrayFloatGL(&v->viewproj, m);
        v->frustum[0].norm[0] = m[3] - m[0];
        v->frustum[0].norm[1] = m[7] - m[4];
        v->frustum[0].norm[2] = m[11] - m[8];
        v->frustum[0].d = m[15] - m[12];

        v->frustum[1].norm[0] = m[3] + m[0];
        v->frustum[1].norm[1] = m[7] + m[4];
        v->frustum[1].norm[2] = m[11] + m[8];
        v->frustum[1].d = m[15] + m[12];

        v->frustum[2].norm[0] = m[3] - m[1];
        v->frustum[2].norm[1] = m[7] - m[5];
        v->frustum[2].norm[2] = m[11] - m[9];
        v->frustum[2].d = m[15] - m[13];

        v->frustum[3].norm[0] = m[3] + m[1];
        v->frustum[3].norm[1] = m[7] + m[5];
        v->frustum[3].norm[2] = m[11] + m[9];
        v->frustum[3].d = m[15] + m[13];

        v->frustum[4].norm[0] = m[3] - m[2];
        v->frustum[4].norm[1] = m[7] - m[6];
        v->frustum[4].norm[2] = m[11] - m[10];
        v->frustum[4].d = m[15] - m[14];

        v->frustum[5].norm[0] = m[3] + m[2];
        v->frustum[5].norm[1] = m[7] + m[6];
        v->frustum[5].norm[2] = m[11] + m[10];
        v->frustum[5].d = m[15] + m[14];

/*
	vec3_t forward, left, up, origin;
	float far, near;
	Matrix4x4_ToVectors(&v->view, up, left, forward, origin);
	far = v->far;
	near = v->near;
//	float heightvec = v->projection.m[1][1];
//	float widthvec = v->projection.m[0][0];
//	float farheight = v->projection.m[1][1]* far; // may need to switch
//	float farwidth = farheight/v->aspect;
	vec3_t farp;
	farp[0] = forward[0] * far + origin[0];
	farp[1] = forward[1] * far + origin[1];
	farp[2] = forward[2] * far + origin[2];
	v->frustum[4].norm[0] = -forward[0];
	v->frustum[4].norm[1] = -forward[1];
	v->frustum[4].norm[2] = -forward[2];
	v->frustum[4].d = dot(v->frustum[4].norm, farp);

	vec3_t nearp;
	nearp[0] = forward[0] * near + origin[0];
	nearp[1] = forward[1] * near + origin[1];
	nearp[2] = forward[2] * near + origin[2];
	v->frustum[5].norm[0] = forward[0];
	v->frustum[5].norm[1] = forward[1];
	v->frustum[5].norm[2] = forward[2];

//	v->frustum[5].d = dot(forward, nearp);
	v->frustum[5].d = dot(nearp, forward);
*/
}

void getDir(viewport_t * v){
	//code inspired by iquilezles volumetric sort
	vec_t *forward = v->v_forward;
	const int   sx = forward[0]<0.0f;
	const int   sz = forward[2]>0.0f;
	const float ax = fabsf(forward[0]);
	const float az = fabsf(forward[2]);

//	ret = 2*sx + 4*sz + (ax > az);

	char * out = v->dir;

	out[0] = sx + 2*sz;
	if(ax > az){
		out[1] = sx + 2*(!sz);
		out[2] = (!sx) + 2*sz;
		out[3] = (!sx) + 2*(!sz);
	} else {
	 	out[1] = (!sx) + 2*sz;
		out[2] = sx + 2*(!sz);
		out[3] = (!sx) + 2*(!sz);
	}
}

int recalcViewport(viewport_t * v, vec3_t pos, vec3_t angle, float fov, float aspect, float near, float far){
	if(pos[0] != v->pos[0] ||
	   pos[1] != v->pos[1] ||
	   pos[2] != v->pos[2] ||
	 angle[0] != v->angle[0]||
	 angle[1] != v->angle[1]||
	 angle[2] != v->angle[2]){
//	if(pos != v->pos || angle != v->angle){
		v->viewchanged = TRUE;
		v->pos[0] = pos[0];
		v->pos[1] = pos[1];
		v->pos[2] = pos[2];
		v->angle[0] = angle[0];
		v->angle[1] = angle[1];
		v->angle[2] = angle[2];
//		recalcViewVectors(v); // done in matrix anyway
		recalcViewMatrix(v);
		Matrix4x4_CreateFromQuakeEntity(&v->cam, pos[0], pos[1], pos[2], angle[2], angle[1], angle[0], 1.0);

	}
	if(fov != v->fov || aspect != v->aspect || v->near!= near || v->far != far){
		v->viewchanged = TRUE;
		v->far = far;
		v->near = near;
		v->aspect = aspect;
		v->fov = fov;
		recalcProjectionMatrix(v);
	}
	if(v->viewchanged){
		Matrix4x4_Concat(&v->viewproj, &v->projection, &v->view);
		recalcFrustum(v);
		recalcFrustumBBoxP(v);
	}
	//TODO change this to projchanged and viewchanged and reset em!
	getDir(v);
	return v->viewchanged;
}
int generateFramebuffersForViewport(viewport_t *v){
	unsigned char deferredflags[] = {3, 7, 3};
	unsigned char deferredrb = FRAMEBUFFERRBFLAGSDEPTH;//todo
	unsigned char deferredcount = 3;
	unsigned char outflags[] = {6};
	unsigned char outrb = 0;
	unsigned char outcount = 1; //todo

	framebuffer_t * dfb;
	framebuffer_t * outfb;
	dfb  = createAndAddFramebufferRPOINT(v->name, deferredcount, deferredrb, deferredflags);
	outfb = createAndAddFramebufferRPOINT(v->name, outcount, outrb, outflags);

	//assuming the framebuffer is still bound
	outfb->rb = dfb->rb;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, outfb->rb);

	v->dfbid = dfb->myid;
	v->outfbid = outfb->myid;
//	v->outfbid->rb = v->dfbid.rb;
	//todo
	return FALSE;
}
int resizeViewport(viewport_t *v, int width, int height){
	if(!v){
		return FALSE;
	}
//	if(v->height == height) return FALSE;
//	if(v->width == v->width) return FALSE;
//	v->height = height;
//	v->width = width;
	float aspect = (float)width/(float)height;
//	if(aspect != v->aspect){
		v->aspect = aspect;
//		recalcViewport(v, v->pos, v->angle, v->fov, aspect, v->near, v->far);
		recalcProjectionMatrix(v);
		Matrix4x4_Concat(&v->viewproj, &v->projection, &v->view);
		recalcFrustum(v);

//	}
	framebuffer_t *outfb = returnFramebufferById(v->outfbid);
	if(!outfb){
		return FALSE;
	}
	framebuffer_t *dfb = returnFramebufferById(v->dfbid);
	if(dfb) resizeFramebuffer(dfb, width, height);

	return resizeFramebuffer(outfb, width, height);
}
