#ifndef VBOMANAGERHEADER
#define VBOMANAGERHEADER

typedef struct vbo_s {
	char * name;
	GLuint vaoid;
	GLuint vboid;// maybe not
	GLuint indicesid;
	GLuint numverts;
	GLuint numfaces;
	char type; // maybe not
}vbo_t;

typedef struct ubo_s {
	char * name;
	GLuint id;// maybe not
	char type; // maybe not
}ubo_t;


int vboOK;
int vbonumber;
int ubonumber;


int initVBOSystem(void);
vbo_t * findVBOByName(char * name);
ubo_t * findUBOByName(char * name);
vbo_t * addVBOToList(vbo_t vbo);
ubo_t * addUBOToList(ubo_t ubo);
vbo_t * createAndAddVBO(char * name, char type);
ubo_t * createAndAddUBO(char * name, char type);
vbo_t createVBO(char *name, char type);
ubo_t createUBO(char *name, char type);
#endif

