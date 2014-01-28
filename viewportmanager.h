#ifndef VIEWPORTMANAGERHEADER
#define VIEWPORTMANAGERHEADER

typedef struct viewport_s
{
	char * name;
	GLuint id;
	GLuint width;
	GLuint height;
	GLuint aspect; // maybe
	GLuint fov;
	int viewchanged;
	matrix4x4_t projection;
	matrix4x4_t view;
} viewport_t;

viewport_t *defaultViewport;
int initViewPortSystem(void);
int vpnumber;
int viewportsOK;

viewport_t * findViewPortByName(char * name);
viewport_t * returnViewport(int it);
viewport_t * addViewportToList(viewport_t vp);
#endif
