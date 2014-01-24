#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>

#include "globaldefs.h"
#include "vbomanager.h"

int vboOK = 0;
int vbonumber = 0;
vbo_t *vbolist;

int initVBOSystem(void){
//	if(!initVAO(&vertexarrayid)) return FALSE;
	vbo_t none = {"default", 0, 0, 0, 0};
	if(vbolist) free(vbolist);
	vbolist = malloc(vbonumber * sizeof(vbo_t));
	if(!vbolist) memset(vbolist, 0 , vbonumber * sizeof(vbo_t));
	addVBOToList(none);
	vboOK = TRUE;
	return TRUE;
}
int addVBOToList(vbo_t vbo){
	int current = vbonumber;
	vbonumber++;
	vbolist = realloc(vbolist, vbonumber*sizeof(vbo_t));
	vbolist[current] = vbo;
	vbolist[current].name = malloc(sizeof(*vbo.name));
	strcpy(vbolist[current].name, vbo.name);
	return current;
}
vbo_t * findVBOByName(char * name){
	int i;
	for (i=0; i<vbonumber; i++){
		if(!strcmp(name, vbolist[i].name)) return &vbolist[i];
	}
	return &vbolist[0];
}
vbo_t * createAndAddVBO(char * name, char type){
	return &vbolist[addVBOToList(createVBO(name, type))];
}
vbo_t createVBO(char * name, char type){
	vbo_t v;
	v.type = 0;
	glGenVertexArrays(1, &v.vaoid);	if(!v.vaoid) return v;
	glBindVertexArray(v.vaoid);
	glGenBuffers(1, &v.vboid);	if(!v.vboid) return v;
	glGenBuffers(1, &v.indicesid);	if(!v.indicesid) return v;
	v.name = malloc(sizeof(*name));
	strcpy(v.name, name);
	v.type = type; //todo type stuff
	return v;
}
