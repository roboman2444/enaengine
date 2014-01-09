#ifndef VBOMANAGERHEADER
#define VBOMANAGERHEADER

typedef struct vbo_s {
	char * name;
	GLuint vaoid;
	GLuint vboid;// maybe not
	GLuint indicesid;
	char type; // maybe not
}vbo_t;



int initVBOSystem(void);
vbo_t * findVBOByName(char * name);
int addVBOToList(vbo_t vbo);
vbo_t * createAndAddVBO(char * name, char type);
vbo_t createVBO(char *name, char type);
#endif

