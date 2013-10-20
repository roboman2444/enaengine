#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER

typedef struct framebuffer_s
{
	GLuint id;
	char * name;
	GLuint width;
	GLuint height;
	GLuint aspect; // maybe
	GLuint fov; // maybe
	GLuint texid;
} framebuffer_t;
int initFrameBufferSystem(void);
//int createAndLoadShader(char * name);
//int addFrameBufferToList(char * name,GLuint id, GLuint vertexid, GLuint fragmentid);

int findFrameBufferByName(char * name);
#endif
