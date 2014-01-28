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
//	texture_t texture;
} framebuffer_t;

framebuffer_t *defaultFrameBuffer;
int initFrameBufferSystem(void);
int fbnumber;
int framebuffersOK;

framebuffer_t * findFrameBufferByName(char * name);
framebuffer_t * returnFrameBuffer(int it);
framebuffer_t * addFrameBufferToList(framebuffer_t fb);
#endif
