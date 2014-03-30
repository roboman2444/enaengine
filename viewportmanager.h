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
	plane_t frustum[6];
} viewport_t;

typedef struct viewportlistpoint_s {
	//gotta free dis stuff after you use it
	viewport_t **list;
	int count;
} viewportlistpoint_t;
typedef struct viewportlistint_s {
	//gotta free dis stuff after you use it
	int *list;
	int count;
} viewportlistint_t;

int initViewportSystem(void);

viewport_t * viewportlist;
int vpnumber;
int viewportsOK;
int viewportcount;
int viewportArraySize;
int viewportArrayLastTaken;

viewport_t * findViewportByNameRPOINT(char * name);
int findViewportByNameRINT(char * name);

viewportlistpoint_t findViewportsByNameRPOINT(char * name);
viewportlistint_t findViewportsByNameRINT(char * name);

viewport_t * returnViewportById(int id);

viewport_t * createAndAddViewportRPOINT(char * name, char type);
int createAndAddViewportRINT(char * name, char type);

viewport_t createViewport(char * name, char type);

int recalcViewport(viewport_t * v, vec3_t pos, vec3_t angle, float fov, float aspect, float near, float far);
int testPointInFrustum(viewport_t *v, vec_t *p);
int testSphereInFrustum(viewport_t *v, vec_t *p, float size);
int testBBoxPointsInFrustum(viewport_t *v, vec_t *points);
#endif
