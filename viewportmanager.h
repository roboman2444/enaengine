#ifndef VIEWPORTMANAGERHEADER
#define VIEWPORTMANAGERHEADER

typedef struct viewport_s
{
	char * name;
	unsigned int id;
	float aspect; // maybe
	float fov;
	float near;
	float far;
	int viewchanged;
	vec3_t pos;
	vec3_t angle;
	matrix4x4_t projection;
	matrix4x4_t view;
	matrix4x4_t viewproj;
} viewport_t;

viewport_t *defaultViewport;
int initViewPortSystem(void);
int vpnumber;
int viewportsOK;

viewport_t * findViewPortByName(char * name);
viewport_t * returnViewport(int it);
viewport_t * addViewportToList(viewport_t vp);
viewport_t createViewport(char * name);
viewport_t * createAndAddViewport(char * name);
int recalcViewport(viewport_t * v, vec3_t pos, vec3_t angle, float fov, float aspect, float near, float far);
#endif
