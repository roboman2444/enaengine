#ifndef FRAMEBUFFERMANAGERHEADER
#define FRAMEBUFFERMANAGERHEADER

typedef struct framebuffer_s
{
	char * name;
	GLuint id;
	GLuint width;
	GLuint height;
	GLuint aspect; // maybe
	GLuint fov; // maybe
	GLuint texid;
} framebuffer_t;
int initFrameBufferSystem(void);

framebuffer_t * findFrameBufferByName(char * name);
framebuffer_t * returnFrameBuffer(int it);
int addFrameBufferToList(framebuffer_t fb);
#endif
