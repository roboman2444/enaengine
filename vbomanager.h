#ifndef VBOMANAGERHEADER
#define VBOMANAGERHEADER

typedef struct vbo_s {
	char * name;
	GLuint vaoid;
	GLuint vboid;// maybe not
	GLuint indicesid;
	GLuint numverts;
	GLuint numfaces;
	int myid;
	char type; // maybe not
}vbo_t;

typedef struct ubo_s {
	char * name;
	GLuint id;// maybe not
	char type; // maybe not
}ubo_t;


int vboOK;
int vbonumber;
vbo_t * vbolist;
int vbocount;
int vboArraySize;
int vboArrayLastTaken;

int initVBOSystem(void);

vbo_t * findVBOByNameRPOINT(char * name);
int findVBOByNameRINT(char * name);

vbo_t * returnVBOById(int id);

vbo_t * createAndAddVBORPOINT(char * name, char type);
int createAndAddVBORINT(char * name, char type);
vbo_t createVBO(char *name, char type);
#endif

