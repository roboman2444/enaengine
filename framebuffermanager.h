#ifndef FRAMEBUFFERMANAGERHEADER
#define FRAMEBUFFERMANAGERHEADER

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

int findFrameBufferByName(char * name);
framebuffer_t returnFrameBuffer(int it);
int addFrameBufferToList(framebuffer_t fb);
#endif
