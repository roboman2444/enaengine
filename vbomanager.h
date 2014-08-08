#ifndef VBOMANAGERHEADER
#define VBOMANAGERHEADER

#define POSATTRIBLOC 0
#define NORMATTRIBLOC 1
#define TCATTRIBLOC 2
#define TANGENTATTRIBLOC 3
#define BLENDIATTRIBLOC 4
#define BLENDWATTRIBLOC 5
#define INSTANCEATTRIBLOC 6

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
	unsigned char tangentstride;
	unsigned char totalstride;
}vbo_t;

typedef struct vboseperate_s {
	char * name;

	GLuint vaoid;
	 // must be in this order or else i have to rewrite deletion functions.
	GLuint vboposid;
	GLuint vbonormid;
	GLuint vbotcid;
	GLuint vbotangentid;
	GLuint vboblendiid;
	GLuint vboblendwid;
	GLuint indicesid;

	GLuint numverts;
	GLuint numfaces;
	int myid;
	char type; // maybe not
	char setup;
}vboseperate_t;

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
void pruneVBOList(void);


int deleteVBO(int vboid);
int setUpVBOStride(vbo_t * vbo, unsigned char posstride, unsigned char normstride, unsigned char tcstride, unsigned char tangentstride, unsigned char blendistride, unsigned char blendwstride);

void statesBindBuffer(const GLenum type, const GLuint id);
void statesBindVertexArray(const GLuint id);
#endif

