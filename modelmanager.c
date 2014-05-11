#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys
#include <math.h>

#include "globaldefs.h"
#include "hashtables.h"
#include "texturemanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "shadermanager.h"
#include "console.h"
#include "mathlib.h"
#include "iqm.h"

int modelcount = 0;
int modelArrayFirstOpen = 0;
int modelArrayLastTaken = -1;
int modelArraySize = 0;
int modelsOK = 0;
model_t *modellist;

hashbucket_t modelhashtable[MAXHASHBUCKETS];
//hashbucket_t * modelhashtable;
//model_t * defaultModel;
#define NUMSTATIC 1
#define NUMANIM 1

char *statictypes[] = {".obj"}; //todo filesys
char *animtypes[] = {".iqm"}; //todo filesys //todo

int makeCubeModel(void){
	model_t m;// = malloc(sizeof(model_T));
//	memset(m, 0, sizeof(model_t));
//	consolePrintf("generating cube\n");
	m.type = 1;
	m.name = malloc(strlen("cube")+1);
	strcpy(m.name, "cube");

	GLfloat * points = malloc(sizeof(GLfloat)*64);
	int i;
	for(i = 0; i < 3; i++){
		m.bbox[i*2] = 1.0;
		m.bbox[(i*2)+1] = -1.0;
	}
	for(i = 0; i < 8; i++){
		points[(i*8)+0] = ((i&1)<<1)-1.0;
		points[(i*8)+1] =  (i&2)    -1.0;
		points[(i*8)+2] = ((i&4)>>1)-1.0;

		m.bboxp[(i*3)+0] = ((i&1)<<1)-1.0;
		m.bboxp[(i*3)+1] =  (i&2)    -1.0;
		m.bboxp[(i*3)+2] = ((i&4)>>1)-1.0;

		points[(i*8)+3] = points[(i*8)+0] * 0.57735;
		points[(i*8)+4] = points[(i*8)+1] * 0.57735;
		points[(i*8)+5] = points[(i*8)+2] * 0.57735;

		points[(i*8)+6] = ((i&1)<<1)-1.0;
		points[(i*8)+7] =  (i&2)    -1.0;

	}
	m.spheresize = 1.7320508075688772935274463415058723669428052538103806;
//	getBBoxFromInterleavedMesh(points, 8, 8, m.bbox);
//	getBBoxpFromBBox(m.bbox, m.bboxp);


	GLuint tris[36] = {
				0, 2, 3,
				0, 3, 1,
				4, 5, 7,
				4, 7, 6,
				2, 0, 4,
				2, 4, 6,
				0, 1, 5,
				0, 5, 4,
				1, 3, 7,
				1, 7, 5,
				3, 2, 6,
				3, 6, 7
	};

	vbo_t * myvbo = createAndAddVBORPOINT(m.name, 1);
	if(!myvbo) return FALSE; // todo free and error handle
	m.vbo = myvbo->myid;


	glBindBuffer(GL_ARRAY_BUFFER, myvbo->vboid);
	glBufferData(GL_ARRAY_BUFFER, 8 * 8 * sizeof(GLfloat), points, GL_STATIC_DRAW);
	myvbo->numverts = 8;
	m.interleaveddata = points;
//	free(interleavedbuffer);

	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);

	glEnableVertexAttribArray(NORMATTRIBLOC);
	glVertexAttribPointer(NORMATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

	glEnableVertexAttribArray(TCATTRIBLOC);
	glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,myvbo->indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 *sizeof(GLuint), tris, GL_STATIC_DRAW);
	myvbo->numfaces = 12;
	m.tris = tris;
	m.numfaces = 12;
	m.numverts = 8;

	return addModelRINT(m);
	return TRUE;
}
int makeCubeModel2(void){
	model_t m;// = malloc(sizeof(model_T));
//	memset(m, 0, sizeof(model_t));
//	consolePrintf("generating cube\n");
	m.type = 1;
	m.name = malloc(strlen("cube2")+1);
	strcpy(m.name, "cube2");

	GLfloat points[192] = {
		-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 0.0,
		1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		1.0,  1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		-1.0,  1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 1.0,

		-1.0, -1.0,  1.0, 1.0, 1.0, 1.0, 0.0, 0.0,
		1.0, -1.0,  1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		1.0,  1.0,  1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		-1.0,  1.0,  1.0, 1.0, 1.0, 1.0, 0.0, 1.0,

		-1.0,  1.0,  1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
//		-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 1.0,
		-1.0,  1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 1.0,
//		-1.0,  1.0,  1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		-1.0, -1.0,  1.0, 1.0, 1.0, 1.0, 0.0, 0.0,

		1.0,  1.0,  1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		1.0,  1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 1.0,
		1.0, -1.0,  1.0, 1.0, 1.0, 1.0, 0.0, 0.0,

		-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 1.0,
		1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		1.0, -1.0,  1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		-1.0, -1.0,  1.0, 1.0, 1.0, 1.0, 0.0, 0.0,

		-1.0,  1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 1.0,
		1.0,  1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		1.0,  1.0,  1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		-1.0,  1.0,  1.0, 1.0, 1.0, 1.0, 0.0, 0.0	};
	int i;
	for(i = 0; i < 3; i++){
		m.bbox[i*2] = 1.0;
		m.bbox[(i*2)+1] = -1.0;
	}
	for(i = 0; i < 8; i++){
		m.bboxp[(i*3)+0] = ((i&1)<<1)-1.0;
		m.bboxp[(i*3)+1] =  (i&2)    -1.0;
		m.bboxp[(i*3)+2] = ((i&4)>>1)-1.0;
	}
	m.spheresize = 1.7320508075688772935274463415058723669428052538103806;
//	getBBoxFromInterleavedMesh(points, 8, 8, m.bbox);
//	getBBoxpFromBBox(m.bbox, m.bboxp);


	GLuint tris[36] = {
//				0, 1, 2,
//				2, 3, 0,
				0, 2, 1,
				2, 0, 3,

				4, 5, 6,
				6, 7, 4,
//				4, 6, 5,
//				6, 4, 7,

				8, 9, 10,
				10,11, 8,
//				8, 10, 9,
//				10,8, 11,

//				12, 13, 14,
//				14, 15, 12,
				12, 14, 13,
				14, 12, 15,

				16, 17, 18,
				18, 19, 16,
//				16, 18, 17,
//				18, 16, 19,

//				20, 21, 22,
//				22, 23, 20
				20, 22, 21,
				22, 20, 23
	};

	vbo_t * myvbo = createAndAddVBORPOINT(m.name, 1);
	if(!myvbo) return FALSE; // todo free and error handle
	m.vbo = myvbo->myid;


	glBindBuffer(GL_ARRAY_BUFFER, myvbo->vboid);
	glBufferData(GL_ARRAY_BUFFER, 192 * sizeof(GLfloat), points, GL_STATIC_DRAW);
	myvbo->numverts = 24;
	m.interleaveddata = points;
//	free(interleavedbuffer);

	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);

	glEnableVertexAttribArray(NORMATTRIBLOC);
	glVertexAttribPointer(NORMATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

	glEnableVertexAttribArray(TCATTRIBLOC);
	glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,myvbo->indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 *sizeof(GLuint), tris, GL_STATIC_DRAW);
	myvbo->numfaces = 12;
	m.tris = tris;
	m.numfaces = 12;
	m.numverts = 24;
	//m.stride = 8; //todo

	return addModelRINT(m);
	return TRUE;
}

int initModelSystem(void){
//	modelhashtable = malloc(MAXHASHBUCKETS * 2* sizeof(hashbucket_t));
	memset(modelhashtable, 0, MAXHASHBUCKETS *sizeof(hashbucket_t));
//	model_t none = {"default", findTextureGroupByName("default"), 0, 0};
	if(modellist) free(modellist);
	modellist = 0;
//	modellist = malloc(modelnumber * sizeof(model_t));
//	if(!modellist) memset(modellist, 0 , modelnumber * sizeof(model_t));
//	addModelRINT("default");
//	defaultModel = addModelToList(none);

	makeCubeModel();
	makeCubeModel2();
	modelsOK = TRUE;
	return TRUE;
}

model_t * findModelByNameRPOINT(char * name){
	return returnModelById(findByNameRINT(name, modelhashtable));
}
int findModelByNameRINT(char * name){ //todo global
	return findByNameRINT(name, modelhashtable);
}

int deleteModel(int id){
	int modelindex = (id & 0xFFFF);
	model_t * m = &modellist[modelindex];
	if(m->myid != id) return FALSE;
	if(!m->name) return FALSE;
	deleteFromHashTable(m->name, id, modelhashtable);
	free(m->name);
	if(m->interleaveddata) free(m->interleaveddata);
	//todo call delete vbo
//TODO
//TODO
//TODO
//TODO
	vbo_t * v = returnVBOById(m->vbo);
	if(v) deleteVBO(v->myid);

	memset(m, 0, sizeof(model_t));
	if(modelindex < modelArrayFirstOpen) modelArrayFirstOpen = modelindex;
	for(; modelArrayLastTaken > 0 && !modellist[modelArrayLastTaken].type; modelArrayLastTaken--);
	return TRUE;
}

int deleteAllModels(void){
	int i, count = 0;
	for(i = 0; i < modelArrayLastTaken; i++){
		model_t *m = &modellist[i];
		count += deleteModel(m->myid);
	}
	memset(modelhashtable, 0, MAXHASHBUCKETS *sizeof(hashbucket_t));
//	model_t none = {"default", findTextureGroupByName("default"), 0, 0};
	if(modellist) free(modellist);
	modellist = 0;
	modelcount = 0;
	modelArrayFirstOpen = 0;
	modelArrayLastTaken = -1;
	modelArraySize = 0;

	return count;
}

model_t * returnModelById(int id){
	int modelindex = (id & 0xFFFF);
	model_t * mod = &modellist[modelindex];
	if(!mod->type) return FALSE;
	if(mod->myid == id) return mod;
	return FALSE;
}


int generateNormalsFromMesh(GLfloat * vertbuffer, GLfloat * normbuffer, GLuint * indices, GLuint indicecount, GLuint vertcount, char type){
	//used http://devmaster.net/posts/6065/calculating-normals-of-a-mesh and darkplaces model_shared.c as a ref
	int i;
	for(i = 0; i < indicecount; i += 3 ){
		float *p1 = &vertbuffer[indices[i+0]*3];
		float *p2 = &vertbuffer[indices[i+1]*3];
		float *p3 = &vertbuffer[indices[i+2]*3];
		float v1[3];
		float v2[3];
		int n;
		for(n = 0; n < 3; n++){
			v1[n] = p2[n] - p1[n];
			v2[n] = p3[n] - p1[n];
		}
		float normal[3];
		normal[0] = v1[1] * v2[2] - v2[1] * v1[2];
		normal[1] = v1[2] * v2[0] - v2[2] * v1[0];
		normal[2] = v1[0] * v2[1] - v2[0] * v1[1];
		//if dontwant to do area weighting, normalize now
		if(!type){
			float length = vec3length(normal);
			normal[0] /= length;
			normal[1] /= length;
			normal[2] /= length;
		}

		float *n1 = &normbuffer[indices[i+0]*3];
		float *n2 = &normbuffer[indices[i+1]*3];
		float *n3 = &normbuffer[indices[i+2]*3];

		for(n = 0; n < 3; n++){
			n1[n] += normal[n];
			n2[n] += normal[n];
			n3[n] += normal[n];
		}
	}
	//go through and renormalize
	if(type <2){
		for(i = 0; i < vertcount; i++){
			float * normal = &normbuffer[i*3];
			float length = vec3length(normal);;
//			length = sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
			normal[0] /= length;
			normal[1] /= length;
			normal[2] /= length;
		}
	}
	return TRUE;
}
float getSphereFromInterleavedMesh(GLfloat * interleavedbuffer, GLuint vertcount, int stride){
	if(stride < 5) return 0;
	float size = 0.0;
	int i;
	for(i = 0; i < vertcount; i++){
		float * vert = &interleavedbuffer[(i*stride)];
		float mysize = vec3length(vert);
		if(mysize > size) size = mysize;
	}
//	consolePrintf("spheresize = %f\n", size);
	return size;
}
int getBBoxFromInterleavedMesh(GLfloat * interleavedbuffer, GLuint vertcount, int stride, vec_t * bbox){
	if(stride < 5) return 0;

	bbox[0] = -3.4028e+38;
	bbox[1] = 3.4028e+38;
	bbox[2] = -3.4028e+38;
	bbox[3] = 3.4028e+38;
	bbox[4] = -3.4028e+38;
	bbox[5] = 3.4028e+38;

	int i;
	for(i = 0; i < vertcount; i++){
		float * vert = &interleavedbuffer[(i*stride)];
		if(vert[0] > bbox[0]) bbox[0] = vert[0];
		else if(vert[0] < bbox[1]) bbox[1] = vert[0];
		if(vert[1] > bbox[2]) bbox[2] = vert[1];
		else if(vert[1] < bbox[3]) bbox[3] = vert[1];
		if(vert[2] > bbox[4]) bbox[4] = vert[2];
		else if(vert[2] < bbox[5]) bbox[5] = vert[2];
	}

/*
	for(i = 0; i < 6; i++){
		consolePrintf("bbox %i:%f\n", i, bbox[i]);
	}
*/
	return TRUE;
}

int normalizeNormalsFromInterleavedMesh(GLfloat * interleavedbuffer, GLuint vertcount, int stride){
	if(stride < 5) return FALSE;
	int i;
	for(i = 0; i < vertcount; i++){
		//todo set up something to have a varialbe offset
		float * normal = &interleavedbuffer[(i*stride)+3];
		float length = vec3length(normal);
		vec3div(normal, normal, length);

	}
	return TRUE;
}
int generateNormalsFromInterleavedMesh(GLfloat * interleavedbuffer, GLuint * indices, GLuint indicecount, GLuint vertcount, int stride, char type){
	//used http://devmaster.net/posts/6065/calculating-normals-of-a-mesh and darkplaces model_shared.c as a ref
	if(stride < 5) return FALSE;
	int i;
	for(i = 0; i < indicecount; i += 3 ){
		//todo set up something to have a varialbe offset
		float *p1 = &interleavedbuffer[indices[i+0]*stride];
		float *p2 = &interleavedbuffer[indices[i+1]*stride];
		float *p3 = &interleavedbuffer[indices[i+2]*stride];
		float v1[3];
		float v2[3];
		int j;
		for(j = 0; j < 3; j++){
			v1[j] = p2[j] - p1[j];
			v2[j] = p3[j] - p1[j];
		}
		float n[3];
		n[0] = v1[1] * v2[2] - v2[1] * v1[2];
		n[1] = v1[2] * v2[0] - v2[2] * v1[0];
		n[2] = v1[0] * v2[1] - v2[0] * v1[1];
		//if dontwant to do area weighting, normalize now
		if(!type){ // type 0, no area weighting
			float length = vec3length(n);
			vec3div(n, n, length);
		}
		//todo set up something to have a varialbe offset
		float *n1 = p1 + 3;
		float *n2 = p2 + 3;
		float *n3 = p3 + 3;

		for(j = 0; j < 3; j++){
			n1[j] += n[j];
			n2[j] += n[j];
			n3[j] += n[j];
		}
	}
	//go through and renormalize
	if(type < 2) normalizeNormalsFromInterleavedMesh(interleavedbuffer, vertcount, stride);
	return TRUE;
}
//todo maybe instead of sampling from indices, sample from newindices
//if i dont, it may cause some "resetting" issues.
GLuint * meshDecimate(GLfloat * interleavedbuffer, GLuint * indices, GLuint indicecount, GLuint vertcount, int stride, float cutdistance, GLuint * returncount){
	if(stride < 5) return FALSE;
	GLuint * newindices = malloc(sizeof(GLuint)*indicecount);
	memcpy(newindices, indices,  sizeof(GLuint)*indicecount);
	int i;
	for(i = 0; i < indicecount; i += 3 ){
		int m;
		for(m = 0; m<3; m++){
			int first = i+m;
			int second = i + ((m+1) % 3);
			//todo set up something to have a varialbe offset
			float *p1 = &interleavedbuffer[(indices[first]*stride)+3];
			float *p2 = &interleavedbuffer[(indices[second]*stride)+3];

			float diff[3];
			diff[0] = p1[0]-p2[0];
			diff[1] = p1[1]-p2[1];
			diff[2] = p1[2]-p2[2];
			float dist = (diff[0] *diff[0]) + (diff[1] * diff[1]) + (diff[2] * diff[2]);
			if(dist > cutdistance){
				float l1 = (p1[0] * p1[0]) + (p1[1] * p1[1]) + (p1[2] * p1[2]);
				float l2 = (p2[0] * p2[0]) + (p2[1] * p2[1]) + (p2[2] * p2[2]);
				int replace;
				int test;
				if(l1>l2){
					replace = indices[first];
					test = indices[second];
				} else {
					replace = indices[second];
					test = indices[first];
				}
				int k;
				for(k = 0; k < indicecount; k++){
					//loop through all triangles, make any verts be other verts
					if(indices[k] == test) newindices[k] = replace;
				}
			}
		}
	}
	indices = realloc(indices, sizeof(GLuint)*indicecount*2);
	//loop through all triangles, make sure the triangles are actually full
	int count = 0;
	for(i = 0; i < indicecount; i += 3 ){
		if(newindices[i] != newindices[i+1] && newindices[i+1] != newindices[i+2] && newindices[i+2] != newindices[i]){
			indices[count+indicecount] = newindices[i];
			indices[count+1+indicecount] = newindices[i+1];
			indices[count+2+indicecount] = newindices[i+2];
			count+=3;
		}
	}
	free(newindices);
	indices = realloc(indices, (count+indicecount) * sizeof(GLuint));
	*returncount = (count + indicecount)/3;
	return indices;
}

int loadiqmmeshes(model_t * m, const struct iqmheader hdr, unsigned char *buf){
//    if(meshdata) return false;
/*
    lilswap((uint *)&buf[hdr.ofs_vertexarrays], hdr.num_vertexarrays*sizeof(iqmvertexarray)/sizeof(uint));
    lilswap((uint *)&buf[hdr.ofs_triangles], hdr.num_triangles*sizeof(iqmtriangle)/sizeof(uint));
    lilswap((uint *)&buf[hdr.ofs_meshes], hdr.num_meshes*sizeof(iqmmesh)/sizeof(uint));
    lilswap((uint *)&buf[hdr.ofs_joints], hdr.num_joints*sizeof(iqmjoint)/sizeof(uint));
*/
//    if(hdr.ofs_adjacency) lilswap((uint *)&buf[hdr.ofs_adjacency], hdr.num_triangles*sizeof(iqmtriangle)/sizeof(uint));

//    meshdata = buf;
	int nummeshes = hdr.num_meshes;
	int numtris = hdr.num_triangles;
	int numverts = hdr.num_vertexes;
	int numjoints = hdr.num_joints;
	float *pos = 0;// = malloc(sizeof(float)*3*numverts);
	float *norm = 0;// = malloc(sizeof(float)*3*numverts);
	float *texcoord = 0;// = malloc(sizeof(float)*3*numverts);
	unsigned int *blendindex;// = malloc
	unsigned int *blendweight;
//    textures = new GLuint[nummeshes];
  //  memset(textures, 0, nummeshes*sizeof(GLuint));
	int i;
//	const char *str = hdr.ofs_text ? (char *)&buf[hdr.ofs_text] : "";
	struct iqmvertexarray *vas = (struct iqmvertexarray *)&buf[hdr.ofs_vertexarrays];
	for(i = 0; i < (int)hdr.num_vertexarrays; i++){
		struct iqmvertexarray va = vas[i];
		switch(va.type){
			case IQM_POSITION: if(va.format != IQM_FLOAT || va.size != 3) return FALSE; pos = (float *)&buf[va.offset]; break;
			case IQM_NORMAL: if(va.format != IQM_FLOAT || va.size != 3) return FALSE; norm = (float *)&buf[va.offset]; break;
			//case IQM_TANGENT: if(va.format != IQM_FLOAT || va.size != 4) return FALSE; intangent = (float *)&buf[va.offset]; lilswap(intangent, 4*hdr.num_vertexes); break;
			case IQM_TEXCOORD: if(va.format != IQM_FLOAT || va.size != 2) return FALSE; texcoord = (float *)&buf[va.offset]; break;
			case IQM_BLENDINDEXES: if(va.format != IQM_UBYTE || va.size != 4) return FALSE; blendindex = (unsigned int *)&buf[va.offset]; break;
			case IQM_BLENDWEIGHTS: if(va.format != IQM_UBYTE || va.size != 4) return FALSE; blendweight = (unsigned int *)&buf[va.offset]; break;
			//case IQM_COLOR: if(va.format != IQM_UBYTE || va.size != 4) return FALSE; incolor = (uchar *)&buf[va.offset]; break;
			default: break;
		}
	}
	if(!pos) return FALSE;
	GLfloat * interleavedbuffer = malloc(8*numverts*sizeof(GLfloat));
	memset(interleavedbuffer, 0 , 8*numverts*sizeof(GLfloat));
	for(i = 0; i < numverts; i++){
		interleavedbuffer[(i*8)+0] = pos[(i*3)+0];
		interleavedbuffer[(i*8)+1] = pos[(i*3)+1];
		interleavedbuffer[(i*8)+2] = pos[(i*3)+2];
		if(norm){
			interleavedbuffer[(i*8)+3] = norm[(i*3)+0];
			interleavedbuffer[(i*8)+4] = norm[(i*3)+1];
			interleavedbuffer[(i*8)+5] = norm[(i*3)+2];
		}
		if(texcoord){
			interleavedbuffer[(i*8)+6] = texcoord[(i*2)+0];
			interleavedbuffer[(i*8)+7] = texcoord[(i*2)+1];
		}
	}

	m->spheresize = getSphereFromInterleavedMesh(interleavedbuffer, numverts, 8);
	m->numverts = numverts;
	getBBoxFromInterleavedMesh(interleavedbuffer, numverts, 8, m->bbox);
	getBBoxpFromBBox(m->bbox, m->bboxp);


	GLuint *tris = (GLuint *)&buf[hdr.ofs_triangles];
	//flipping faces... temp fix
	for(i = 0; i < numtris; i++){
		GLuint temp = tris[i*3];
		tris[i*3] = tris[(i*3)+1];
		tris[(i*3)+1] = temp;
	}

	vbo_t * myvbo = createAndAddVBORPOINT(m->name, 1);
	if(!myvbo) return 0; // todo free and error handle
	m->vbo = myvbo->myid;


	glBindBuffer(GL_ARRAY_BUFFER, myvbo->vboid);
	glBufferData(GL_ARRAY_BUFFER, numverts * 8 * sizeof(GLfloat), interleavedbuffer, GL_STATIC_DRAW);
	myvbo->numverts = numverts;
	m->interleaveddata = interleavedbuffer;
//	free(interleavedbuffer);

	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);

	glEnableVertexAttribArray(NORMATTRIBLOC);
	glVertexAttribPointer(NORMATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

	glEnableVertexAttribArray(TCATTRIBLOC);
	glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,myvbo->indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,numtris * 3 *sizeof(GLuint), tris, GL_STATIC_DRAW);
	myvbo->numfaces = numtris;
	m->tris = tris;
	m->numfaces = numtris;
	//m->stride = 8; //todo
//	free(tris);

	consolePrintf("Model %s.iqm has %i faces and %i verts\n", m->name, numtris, numverts);




/*	meshes = (iqmmesh *)&buf[hdr.ofs_meshes];
	joints = (iqmjoint *)&buf[hdr.ofs_joints];
	if(hdr.ofs_adjacency) adjacency = (iqmtriangle *)&buf[hdr.ofs_adjacency];

	baseframe = new Matrix3x4[hdr.num_joints];
	inversebaseframe = new Matrix3x4[hdr.num_joints];
	for(int i = 0; i < (int)hdr.num_joints; i++){
		iqmjoint &j = joints[i];
		baseframe[i] = Matrix3x4(Quat(j.rotate).normalize(), Vec3(j.translate), Vec3(j.scale));
		inversebaseframe[i].invert(baseframe[i]);
		if(j.parent >= 0){
			baseframe[i] = baseframe[j.parent] * baseframe[i];
			inversebaseframe[i] *= inversebaseframe[j.parent];
		}
	}
*/

/*
	for(int i = 0; i < (int)hdr.num_meshes; i++){
		struct iqmmesh &m = meshes[i];
		consolePrintf("%s: loaded mesh: %s\n", filename, &str[m.name]);
//        textures[i] = loadtexture(&str[m.material], 0);
  //      if(textures[i]) printf("%s: loaded material: %s\n", filename, &str[m.material]);
	}
*/
	return TRUE;
}





int loadModelIQM(model_t *m, char * filename){
	//mostly copied from the sdk
	FILE *f = fopen(filename, "rb");
	if(!f) return FALSE;

	unsigned char *buf = NULL;
	struct iqmheader hdr;
	if(fread(&hdr, 1, sizeof(hdr), f) != sizeof(hdr) || memcmp(hdr.magic, IQM_MAGIC, sizeof(hdr.magic)))
		goto error; //spaghetti!
	if(hdr.version != IQM_VERSION)goto error;
	if(hdr.filesize > (16<<20)) goto error; // sanity check... don't load files bigger than 16 MB
	buf = malloc(hdr.filesize);
	if(fread(buf + sizeof(hdr), 1, hdr.filesize - sizeof(hdr), f) != hdr.filesize - sizeof(hdr))
		goto error;

	if(hdr.num_meshes > 0 && !loadiqmmeshes(m, hdr, buf)) goto error;
//	if(hdr.num_anims > 0 && !loadiqmanims(filename, hdr, buf)) goto error;
	fclose(f);
	return TRUE;
	//todo

	error:
	consolePrintf("%s: error while loading\n", filename);
//	if(buf != meshdata && buf != animdata) free(buf);
	fclose(f);
	return FALSE;
	//todo
}








int loadModelOBJ(model_t * m, char * filename){//todo flags
	unsigned int vertcount = 0;
	unsigned int tccount = 0;
	unsigned int normcount = 0;
	unsigned int facecount = 0;

	FILE *f;
	if(!(f = fopen(filename, "r"))) return FALSE;
	char * line = malloc(300*sizeof(char)); //max size of 300;
//	char * testline;
	int over;
	while(fgets(line, 300, f)){
//		testline = line;
//		while((*testline == ' ' || *testline == '\t') && testline < line + 200) testline+=sizeof(char); //should be 1 //take off early white spaces
//		if(testline > 200 + line) continue;
//		if(!strncmp(testline, "v ", 2)) vertcount++;
//		else if(!strncmp(testline, "vt", 2)) tccount++;
//		else if(!strncmp(testline, "vp", 2)) normcount++;
//		else if(!strncmp(testline, "f ", 2)) facecount++;
		for(over = 0; (line[over] == ' ' || line[over] == '\t') && over < 200; over++); //should be 1 //take off early white spaces
		if(!strncmp(line+over, "v ", 2)) vertcount++;
		else if(!strncmp(line+over, "vt", 2)) tccount++;
		else if(!strncmp(line+over, "vp", 2)) normcount++;
		else if(!strncmp(line+over, "f ", 2)) facecount++;

	}

//	if(vertcount + tccount + normcount != vertcount*3) return FALSE;
	if(!vertcount) return FALSE; // no verts


	int addcount = 0;
	if(normcount>tccount && normcount > vertcount){
		addcount = normcount - vertcount;
	} else if(tccount > vertcount) {
		addcount = tccount - vertcount;
	}

	float 	* vertbuffer = malloc(3*vertcount*sizeof(float));
	float 	* tcbuffer = malloc(2*tccount*sizeof(float));
	float 	* normbuffer = malloc(3*normcount*sizeof(float));
	//GLuint  * facebuffer = malloc(9*facecount*sizeof(GLuint)); //one for verts, tc, and normals
	int 	* facebuffer = malloc(9*sizeof(int)); //one for verts, tc, and normals
	GLuint 	* indicebuffer = malloc(3*facecount*sizeof(GLuint));
	GLfloat * interleavedbuffer = malloc(8*facecount*sizeof(GLfloat));
	GLfloat * interleavedaddon = malloc(8*addcount*sizeof(GLfloat));




	memset(vertbuffer, 0, 3*vertcount*sizeof(float));
	memset(normbuffer, 0, 3*normcount*sizeof(float));
	memset(tcbuffer,   0, 2*  tccount*sizeof(float));
	memset(facebuffer, 0, 9*sizeof(int));
	memset(indicebuffer, 0, 3*facecount*sizeof(GLuint));
	memset(interleavedbuffer, 0, 8*facecount*sizeof(GLfloat));
	memset(interleavedaddon, 0, 8*addcount*sizeof(GLfloat));


	rewind(f);


	unsigned int readvert = 0;
	unsigned int readnorm = 0;
	unsigned int readtc = 0;
	unsigned int readface = 0;
//	unsigned int readadd = 0;
	while(fgets(line, 300, f)){
		for(over = 0; (line[over] == ' ' || line[over] == '\t') && over < 200; over++); //should be 1 //take off early white spaces


//		if(!strncmp(testline, "v ", 2)){
		if(!strncmp(line+over, "v ", 2)){
			if(readvert >= vertcount) break; //todo debug
			sscanf(line," v %f %f %f",&vertbuffer[(readvert*3)],&vertbuffer[(readvert*3)+1],&vertbuffer[(readvert*3)+2]);
			readvert++;
		}
		else if(!strncmp(line+over, "vt", 2)){
			if(readtc >= tccount) break; //todo debug
			sscanf(line," vt %f %f",&tcbuffer[(readtc*2)],&tcbuffer[(readtc*2)+1]);
			readtc++;
		}
		else if(!strncmp(line+over, "vp", 2)){
			if(readnorm >= normcount) break; //todo debug
			sscanf(line," vp %f %f %f",&normbuffer[(readnorm*3)],&normbuffer[(readnorm*3)+1], &normbuffer[(readnorm*3)+2]);
			readnorm++;
		}
		else if(!strncmp(line+over, "f ", 2)){
			if(readface >= facecount) break;
//			printf("%i\n",readface);
			if(sscanf(line," f %d/%d/%d %d/%d/%d %d/%d/%d",
				&facebuffer[0], &facebuffer[1], &facebuffer[2],
				&facebuffer[3], &facebuffer[4], &facebuffer[5],
				&facebuffer[6], &facebuffer[7], &facebuffer[8]
			) == 9);
			else if(sscanf(line," f %d/%d %d/%d %d/%d",
				&facebuffer[0], &facebuffer[1],
				&facebuffer[3], &facebuffer[4],
				&facebuffer[6], &facebuffer[7]
			) == 6 ) facebuffer[2] = facebuffer[5] = facebuffer[8] = 0; // make sure they are 0s
			else if(sscanf(line," f %d//%d %d//%d %d//%d",
				&facebuffer[0], &facebuffer[2],
				&facebuffer[3], &facebuffer[5],
				&facebuffer[6], &facebuffer[8]
			) == 6) facebuffer[1] = facebuffer[4] = facebuffer[7] = 0; // make sure they are 0s
			else if(sscanf(line," f %d %d %d",
				&facebuffer[0], &facebuffer[3], &facebuffer[6]
			) == 3) facebuffer[1] = facebuffer[2] = facebuffer[4] = facebuffer[5] = facebuffer[7] = facebuffer[8] = 0; // make sure they are 0s
			else{
				return 0; //todo debug... most likely case is that it has a quad
			}

			int n;
			for(n = 0; n < 3; n++){ //two or more, use a for
				int vindice = 0;
				int tcindice = 0;
				int normindice = 0;
				if(facebuffer[(n*3)+0] > 0) vindice = facebuffer[(n*3)+0];
				else if(facebuffer[(n*3)+0] < 0) vindice = readvert+facebuffer[(n*3)+0];
				else break; //0, todo debug
				if(facebuffer[(n*3)+1] >= 0) tcindice = facebuffer[(n*3)+1];
				else if(facebuffer[(n*3)+1] < 0) tcindice = readtc+facebuffer[(n*3)+1];

				if(facebuffer[(n*3)+2] >= 0) normindice = facebuffer[(n*3)+2];
				else if(facebuffer[(n*3)+2] < 0) normindice = readnorm+facebuffer[(n*3)+2];



				vindice--; //1 to 0
				tcindice--;
				normindice--;

				//may not be doing this properly...
				indicebuffer[(readface*3)+n] = vindice;
				interleavedbuffer[(vindice*8)+0] = vertbuffer[(vindice*3)+0];
				interleavedbuffer[(vindice*8)+1] = vertbuffer[(vindice*3)+1];
				interleavedbuffer[(vindice*8)+2] = vertbuffer[(vindice*3)+2];
				if(normindice>=0){
/*					if(interleavedaddon){
						GLfloat def[3];
						def[0] = interleavedbuffer[(vindice*8)+3];
						def[1] = interleavedbuffer[(vindice*8)+4];
						def[2] = interleavedbuffer[(vindice*8)+5];
						if(def[0] && def[1] && def[2]){
							//new 
						} else {
							interleavedbuffer[(vindice*8)+3] = vertbuffer[(normindice*3)+0];
							interleavedbuffer[(vindice*8)+4] = vertbuffer[(normindice*3)+1];
							interleavedbuffer[(vindice*8)+5] = vertbuffer[(normindice*3)+2];
						}
					} else {*/
						interleavedbuffer[(vindice*8)+3] = vertbuffer[(normindice*3)+0];
						interleavedbuffer[(vindice*8)+4] = vertbuffer[(normindice*3)+1];
						interleavedbuffer[(vindice*8)+5] = vertbuffer[(normindice*3)+2];
//					}
				}
				if(tcindice>=0){
					//todo check if there is maybe something there
//					if(interleavedaddon){
//					} else {
						interleavedbuffer[(vindice*8)+6] = tcbuffer[(tcindice*2)+0];
						interleavedbuffer[(vindice*8)+7] = tcbuffer[(tcindice*2)+1];
//					}
				}
			}
			readface++;
		}
	}


	free(line);
	fclose(f);
	free(vertbuffer);
	free(normbuffer);
	free(facebuffer);
	free(tcbuffer);

	consolePrintf("Model %s has %i faces and %i verts\n", filename, readface, readvert);
//	int print;
//	for(print = 0; print < facecount*3; printf("%i ", indicebuffer[print++]));
//	printf("\n\n\n\n\n\n\n\n\n\n%ix%i data: ", vertcount, readvert);
//	for(print = 0; print < vertcount*8; printf("%f ", interleavedbuffer[print++]));
//	printf("\n");

	if(readface != facecount) return 0; //todo actually debug and free crap
	if(readtc != tccount) return 0; //todo actually debug and whatnot
	if(readvert != vertcount) return 0; //todo actually debug and whatnot
	if(readnorm != normcount) return 0; //todo actually debug and whatnot






/*
	float muhverts[32] = {  -0.5, -0.5, 0.0,  0.0, 0.0, 0.0,  0.0, 0.0,
				-0.5,  0.5, 0.0,  0.0, 0.0, 0.0,  0.0, 1.0,
				 0.5,  0.5, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0,
				 0.5, -0.5, 0.0,  0.0, 0.0, 0.0,  1.0, 0.0};
	int muhindices[6] = { 0, 1, 2, 0, 2, 3};
*/
	//todo actually set flags for if gen verts is needed, and if it should area weight
	if(!readnorm){
//		consolePrintf("Generating vertex normals for Model %s with area weighting\n", filename);
		consolePrintf("Generating vertex normals for Model %s\n", filename);
		generateNormalsFromInterleavedMesh(interleavedbuffer, indicebuffer, facecount*3, vertcount, 8 , 0);
	}
	m->spheresize = getSphereFromInterleavedMesh(interleavedbuffer, vertcount, 8);
	getBBoxFromInterleavedMesh(interleavedbuffer, vertcount, 8, m->bbox);
	getBBoxpFromBBox(m->bbox, m->bboxp);

//	m->bbox = getBBoxFromInterleavedMesh(interleavedbuffer, vertcount, 8);
//	m->numfaces = malloc(sizeof(GLuint)*2);
//	m->numlod = 2;
//	m->numfaces[0] = facecount;
//	GLuint totalface;
//	indicebuffer = meshDecimate(interleavedbuffer, indicebuffer, facecount*3, vertcount, 8 , 0.0001, &totalface);
//	m->numfaces[1] = totalface;
//	consolePrintf("Model %s has %d total triangles for LOD\n", filename, totalface);


//	normalizeNormalsFromInterleavedMesh(interleavedbuffer, vertcount, 8);


	vbo_t * myvbo = createAndAddVBORPOINT(m->name, m->type);
	if(!myvbo) return 0; // todo free and error handle
	m->vbo = myvbo->myid;
//	printf("%i\n",m->vbo->type);
//	printf("%i\n",m->vbo->vaoid);
//	printf("%i\n",m->vbo->vboid);
//	printf("%i\n",m->vbo->indicesid);
//	printf("%s\n",m->name);
//	printf("%s\n",m->vbo->name);
	//the correct vao should be bound at this point.
//	printf("%s vao: %i\n", m->name, m->vbo->vaoid);
//	glBindVertexArray(m->vbo->vaoid);
	glBindBuffer(GL_ARRAY_BUFFER,myvbo->vboid);
	glBufferData(GL_ARRAY_BUFFER, vertcount * 8 * sizeof(GLfloat), interleavedbuffer, GL_STATIC_DRAW);
	myvbo->numverts = vertcount;
	m->numverts = vertcount;
	m->interleaveddata = interleavedbuffer;
//	free(interleavedbuffer);

//	shaderprogram_t * program = findProgramByName("staticmodel");//todo per model materials and permutations
//	printf("\n%i\n", program->id);
//	if(!program->id) return FALSE; //todo debug
//	glUseProgram(program->id);

//	GLint posattrib = findShaderAttribPos(program, "position"); //todo per model materials?
//	printf("%i\n", posattrib);
	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);

//	GLint normattrib = findShaderAttribPos(program, "normal"); //todo per model materials?
//	printf("%i\n", normattrib);
	glEnableVertexAttribArray(NORMATTRIBLOC);
	glVertexAttribPointer(NORMATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

//	GLint tcattrib = findShaderAttribPos(program, "texCoord"); //todo per model materials?
//	printf("%i\n", tcattrib);
	glEnableVertexAttribArray(TCATTRIBLOC);
	glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,myvbo->indicesid);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(*indicebuffer), indicebuffer, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,facecount * 3 *sizeof(GLuint), indicebuffer, GL_STATIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER,totalface * 3 *sizeof(GLuint), indicebuffer, GL_STATIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER,6 * sizeof(GLint), muhindices, GL_STATIC_DRAW);
	myvbo->numfaces = facecount;
	m->tris = indicebuffer;
	m->numfaces = facecount;
//	m->stride = 8;
//	free(indicebuffer);

	//maybe use material based shading

	//todo set flags in the model
	//todo curse more at obj for being stupid
	return TRUE;
}
model_t createAndLoadModel(char * name){
	model_t m;
	m.type = 0; // error
	m.name = malloc(strlen(name)+1);
	strcpy(m.name, name);

	char * filename = malloc(200); //todo filesys and define a size
	struct stat s;
	int n;
	for(n = 0; n < NUMSTATIC &&  statictypes[n]; n++){
		sprintf(filename, "%s%s", name, statictypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			if(!loadModelOBJ(&m, filename)){
				free(filename);
				 return m;
			}
			m.type = 1;
			free(filename);
			return m;
		}
	}
	for(n = 0; n < NUMANIM &&  animtypes[n]; n++){
		sprintf(filename, "%s%s", name, animtypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			//TODO CALL SOME SORTA LOADING FUNCTION... ANIMATED
			if(!loadModelIQM(&m, filename)){
				free(filename);
				return m;
			}
			m.type = 1; //todo
			free(filename);
			return m;
		}
	}
	if(!m.type)/*some sorta error*/;

	free(filename);
	return m;
}

//if i want to load a static iqm model forsay
model_t createAndLoadTypeModel(char * name, char type){
	model_t m;
	m.type = 0; // error
	m.name = malloc(strlen(name)+1);
	strcpy(m.name, name);

	char * filename = malloc(200); //todo filesys and define a size
	struct stat s;
	int n;
	for(n = 0; n < sizeof(statictypes) &&  statictypes[n]; n++){
		sprintf(filename, "%s%s", name, statictypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			if(!loadModelOBJ(&m, filename)){
				free(filename);
				return m;
			}

			m.type = type;
			free(filename);

			return m;
		}
	}
	for(n = 0; n < sizeof(animtypes) &&  animtypes[n]; n++){
		sprintf(filename, "%s%s", name, animtypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			if(!loadModelIQM(&m, filename)){
				free(filename);
				return m;
			}
			m.type = type;
			free(filename);
			return m;
		}
	}
	if(!m.type)/*some sorta error*/;

	free(filename);
	return m;
}





int addModelRINT(model_t mod){
	modelcount++;
	for(; modelArrayFirstOpen < modelArraySize && modellist[modelArrayFirstOpen].type; modelArrayFirstOpen++);
	if(modelArrayFirstOpen == modelArraySize){	//resize
		modelArraySize++;
		modellist = realloc(modellist, modelArraySize * sizeof(model_t));
	}
	modellist[modelArrayFirstOpen] = mod;
	int returnid = (modelcount << 16) | modelArrayFirstOpen;
	modellist[modelArrayFirstOpen].myid = returnid;

	addToHashTable(modellist[modelArrayFirstOpen].name, returnid, modelhashtable);
	if(modelArrayLastTaken < modelArrayFirstOpen) modelArrayLastTaken = modelArrayFirstOpen; //todo redo
	return returnid;
}
model_t * addModelRPOINT(model_t mod){
	modelcount++;
	for(; modelArrayFirstOpen < modelArraySize && modellist[modelArrayFirstOpen].type; modelArrayFirstOpen++);
	if(modelArrayFirstOpen == modelArraySize){	//resize
		modelArraySize++;
		modellist = realloc(modellist, modelArraySize * sizeof(model_t));
	}
	modellist[modelArrayFirstOpen] = mod;
	int returnid = (modelcount << 16) | modelArrayFirstOpen;
	modellist[modelArrayFirstOpen].myid = returnid;

	addToHashTable(modellist[modelArrayFirstOpen].name, returnid, modelhashtable);
	//todo maybe have model have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(modelArrayLastTaken < modelArrayFirstOpen) modelArrayLastTaken = modelArrayFirstOpen;
//	printf("modelarraysize = %i\n", modelArraySize);
//	printf("modelcount = %i\n", modelcount);

	return &modellist[modelArrayFirstOpen];

}
int createAndAddModelRINT(char * name){
	int m = findModelByNameRINT(name);
	if(m) return m;
	return addModelRINT(createAndLoadModel(name));
//	return &modellist[addModelToList(createAndLoadModel(name))];
}
model_t * createAndAddModelRPOINT(char * name){
	model_t * m = findModelByNameRPOINT(name);
	if(m) return m;
	return addModelRPOINT(createAndLoadModel(name));
//	return &modellist[addModelToList(createAndLoadModel(name))];
}

