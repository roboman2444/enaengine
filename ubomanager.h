#ifndef UBOMANAGERHEADER
#define UBOMANAGERHEADER

#define UBOBLOCKLOC 0
/*
typedef struct vbo_s {
	char * name;

	GLuint vaoid;
	 // must be in this order or else i have to rewrite deletion functions.
	GLuint vboid;
	GLuint indicesid;

	GLuint numverts;
	GLuint numfaces;
	int myid;
	char type; // maybe not

	char setup;

	unsigned char posstride;
	unsigned char normstride;
	unsigned char tcstride;
	unsigned char totalstride;
}vbo_t;

typedef struct ubo_s {
	char * name;
	GLuint id;// maybe not
	char type; // maybe not
}ubo_t;
*/

GLint maxUBOSize;
GLint uboAlignment;

int ubo_ok;
//int ubonumber;
//ubo_t * ubolist;
//int ubocount;
//int uboArraySize;
//int uboArrayLastTaken;

int ubo_init(void);
/*
vbo_t * findVBOByNameRPOINT(char * name);
int findVBOByNameRINT(char * name);

vbo_t * returnVBOById(int id);

vbo_t * createAndAddVBORPOINT(char * name, char type);
int createAndAddVBORINT(char * name, char type);
vbo_t createVBO(char *name, char type);

int deleteVBO(int vboid);
int setUpVBO(vbo_t * vbo, unsigned char posstride, unsigned char normstride, unsigned char tcstride);
*/
#endif

