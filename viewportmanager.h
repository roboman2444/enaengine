#ifndef VIEWPORTMANAGERHEADER
#define VIEWPORTMANAGERHEADER

typedef struct viewport_s {
	char * name;
	char type;
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
	int myid;
} viewport_t;

int initViewPortSystem(void);

viewport_t * viewportlist;
int vpnumber;
int viewportsOK;
int viewportcount;
int viewportArraySize;
int viewportArrayLastTaken;

viewport_t * findViewportByNameRPOINT(char * name);
int findViewportByNameRINT(char * name);

viewport_t * returnViewportById(int id);

viewport_t * createAndAddViewportRPOINT(char * name, char type);
int createAndAddViewportRINT(char * name, char type);

viewport_t createViewport(char * name, char type);

int recalcViewport(viewport_t * v, vec3_t pos, vec3_t angle, float fov, float aspect, float near, float far);
#endif
