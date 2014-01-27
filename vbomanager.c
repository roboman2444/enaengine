#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>

#include "globaldefs.h"
#include "vbomanager.h"

int vboOK = 0;
int vbonumber = 0;
int ubonumber = 0;
vbo_t *vbolist;
ubo_t *ubolist;

int initVBOSystem(void){
//	if(!initVAO(&vertexarrayid)) return FALSE;
	vbo_t vbonone = {"default", 0, 0, 0, 0};
	if(vbolist) free(vbolist);
	vbolist = malloc(vbonumber * sizeof(vbo_t));
	if(!vbolist) memset(vbolist, 0 , vbonumber * sizeof(vbo_t));
	addVBOToList(vbonone);

	ubo_t ubonone = {"default", 0, 0};
	if(ubolist) free(ubolist);
	ubolist = malloc(ubonumber * sizeof(ubo_t));
	if(!ubolist) memset(ubolist, 0 , ubonumber * sizeof(ubo_t));
	addUBOToList(ubonone);

	vboOK = TRUE;
	return TRUE;
}
int addVBOToList(vbo_t vbo){
	int current = vbonumber;
	vbonumber++;
	vbolist = realloc(vbolist, vbonumber*sizeof(vbo_t));
	vbolist[current] = vbo;
//	vbolist[current].name = malloc(strlen(vbo.name)+1);
//	strcpy(vbolist[current].name, vbo.name);
	return current;
}
int addUBOToList(ubo_t ubo){
	int current = ubonumber;
	ubonumber++;
	ubolist = realloc(ubolist, ubonumber*sizeof(ubo_t));
	ubolist[current] = ubo;
//	ubolist[current].name = malloc(strlen(ubo.name)+1);
//	strcpy(ubolist[current].name, ubo.name);
	return current;
}

vbo_t * findVBOByName(char * name){
	int i;
	for (i=0; i<vbonumber; i++){
		if(!strcmp(name, vbolist[i].name)) return &vbolist[i];
	}
	return &vbolist[0];
}
ubo_t * findUBOByName(char * name){
	int i;
	for (i=0; i<ubonumber; i++){
		if(!strcmp(name, ubolist[i].name)) return &ubolist[i];
	}
	return &ubolist[0];
}

vbo_t * createAndAddVBO(char * name, char type){
	return &vbolist[addVBOToList(createVBO(name, type))];
}
ubo_t * createAndAddUBO(char * name, char type){
	return &ubolist[addUBOToList(createUBO(name, type))];
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
	v.type = type; //todo type stuff
	return v;
}
ubo_t createUBO(char * name, char type){
	ubo_t u;
	u.type = 0;
	//todo fill this in more
//	glGenVertexArrays(1, &v.vaoid);	if(!v.vaoid) return v;
//	glBindVertexArray(v.vaoid);
	glGenBuffers(1, &u.id);	if(!u.id) return u;
//	glGenBuffers(1, &v.indicesid);	if(!v.indicesid) return v;
	u.name = malloc(strlen(name)+1);
	strcpy(u.name, name);
	u.type = type; //todo type stuff
	return u;
}
