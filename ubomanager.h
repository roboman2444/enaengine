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
typedef struct ubo_s {
	int myid;
	GLuint id;
	unsigned int place;
//	unsigned int oldplace;
	unsigned int size;
	GLubyte *data;
	char type;
} ubo_t;

GLint maxUBOSize;
GLint uboAlignment;

int ubo_ok;
//int ubonumber;
//ubo_t * ubolist;
//int ubocount;
//int uboArraySize;
//int uboArrayLastTaken;

int ubo_init(void);
char ubo_flushData(ubo_t *u);
int ubo_pushData(ubo_t *u, const unsigned int size, const void * data);

unsigned int ubo_halfData(ubo_t * u);
unsigned int ubo_sizeData(ubo_t * u, const unsigned int newsize);
unsigned int ubo_pruneData(ubo_t * u);
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
