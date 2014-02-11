#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>

#include "globaldefs.h"
#include "vbomanager.h"
#include "hashtables.h"

int vboOK = 0;
int vbocount = 0;
int ubocount = 0;
int vboArrayFirstOpen = 0;
int vboArrayLastTaken = 0;
int vboArraySize = 0;
int uboArrayFirstOpen = 0;
int uboArrayLastTaken = 0;
int uboArraySize = 0;
vbo_t *vbolist;
ubo_t *ubolist;

hashbucket_t vbohashtable[MAXHASHBUCKETS];
//hashbucket_t ubohashtable[MAXHASHBUCKETS];

int initVBOSystem(void){
//	if(!initVAO(&vertexarrayid)) return FALSE;
//	vbo_t vbonone = {"default", 0, 0, 0, 0};
	memset(vbohashtable, 0, MAXHASHBUCKETS*sizeof(hashbucket_t));
//	memset(ubohashtable, 0, MAXHASHBUCKETS*sizeof(hashbucket_t));
	if(vbolist) free(vbolist);
	vbolist = malloc(vboArraySize * sizeof(vbo_t));
	if(!vbolist) memset(vbolist, 0 , vbonumber * sizeof(vbo_t));
//	addVBOToList(vbonone);

	vboOK = TRUE;
	return TRUE;
}


vbo_t * findVBOByNameRPOINT(char * name){
	return returnVBOById(findByNameRINT(name, vbohashtable));
}
int findVBOByNameRINT(char * name){
	return findByNameRINT(name, vbohashtable);
}

int deleteVBO(int id){
	int vboindex = (id & 0xFFFF);
	vbo_t * vbo = &vbolist[vboindex];
	if(vbo->myid != id) return FALSE;
	if(!vbo->name) return FALSE;
	deleteFromHashTable(vbo->name, id, vbohashtable);
	free(vbo->name);
	bzero(vbo, sizeof(vbo_t));
	if(vboindex < vboArrayFirstOpen) vboArrayFirstOpen = vboindex;
	for(; vboArrayLastTaken > 0 && !vbolist[vboArrayLastTaken].type; vboArrayLastTaken--);
	return TRUE;
}

vbo_t * returnVBOById(int id){
	int vboindex = (id & 0xFFFF);
	vbo_t * vbo = &vbolist[vboindex];
	if(!vbo->type) return FALSE;
	if(vbo->myid == id) return vbo;
	return FALSE;
}
vbo_t createVBO(char * name, char type){
	vbo_t v;
	v.type = 0;
	glGenVertexArrays(1, &v.vaoid);	if(!v.vaoid) return v;
	glBindVertexArray(v.vaoid);
	glGenBuffers(1, &v.vboid);	if(!v.vboid) return v;
	glGenBuffers(1, &v.indicesid);	if(!v.indicesid) return v;
	v.name = malloc(strlen(name)+1);
	strcpy(v.name, name);
//	v.type = type; //todo type stuff
	v.type = 2;
	return v;
}

int addVBORINT(vbo_t vbo){
	vbocount++;
	for(; vboArrayFirstOpen < vboArraySize && vbolist[vboArrayFirstOpen].type; vboArrayFirstOpen++);
	if(vboArrayFirstOpen == vboArraySize){	//resize
		vboArraySize++;
		vbolist = realloc(vbolist, vboArraySize * sizeof(vbo_t));
	}
	vbolist[vboArrayFirstOpen] = vbo;
	int returnid = (vbocount << 16) | vboArrayFirstOpen;
	vbolist[vboArrayFirstOpen].myid = returnid;

	addToHashTable(vbolist[vboArrayFirstOpen].name, returnid, vbohashtable);
	if(vboArrayLastTaken < vboArrayFirstOpen) vboArrayLastTaken = vboArrayFirstOpen; //todo redo
	return returnid;
}
vbo_t * addVBORPOINT(vbo_t vbo){
	vbocount++;
	for(; vboArrayFirstOpen < vboArraySize && vbolist[vboArrayFirstOpen].type; vboArrayFirstOpen++);
	if(vboArrayFirstOpen == vboArraySize){	//resize
		vboArraySize++;
		vbolist = realloc(vbolist, vboArraySize * sizeof(vbo_t));
	}
	vbolist[vboArrayFirstOpen] = vbo;
	int returnid = (vbocount << 16) | vboArrayFirstOpen;
	vbolist[vboArrayFirstOpen].myid = returnid;

	addToHashTable(vbolist[vboArrayFirstOpen].name, returnid, vbohashtable);
	if(vboArrayLastTaken < vboArrayFirstOpen) vboArrayLastTaken = vboArrayFirstOpen; //todo redo
	return &vbolist[vboArrayFirstOpen];
}

vbo_t * createAndAddVBORPOINT(char * name, char type){
	return addVBORPOINT(createVBO(name, type));
}
int createAndAddVBORINT(char * name, char type){
	return addVBORINT(createVBO(name, type));
}
