#ifndef VIEWPORTMANAGERHEADER
#define VIEWPORTMANAGERHEADER

typedef struct viewport_s {
	char * name;
	char type;
	int myid;
	float aspect; // maybe
	float fov;
	float near;
	float far;
	int viewchanged;
	int outfbid; //todo add more for ssao, deferred, etc
	int dfbid; //todo add more for ssao, deferred, etc
	vec3_t pos;
	vec3_t angle;
	matrix4x4_t projection;
	matrix4x4_t view;
	matrix4x4_t viewproj;
	plane_t frustum[6];

	vec3_t v_forward;
	vec3_t v_up;
	vec3_t v_right;

	char dir[4];
} viewport_t;

typedef struct viewportlistpoint_s {
	//gotta free dis stuff after you use it
	viewport_t **list;
	unsigned int count;
} viewportlistpoint_t;
typedef struct viewportlistint_s {
	//gotta free dis stuff after you use it
	int *list;
	unsigned int count;
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

vec_t distPointFromNear(viewport_t *v, vec_t *p);

int recalcViewport(viewport_t * v, vec3_t pos, vec3_t angle, float fov, float aspect, float near, float far);

int testPointInFrustum(viewport_t *v, vec_t *p);
int testSphereInFrustum(viewport_t *v, vec_t *p, float size);
int testBBoxPInFrustum(viewport_t *v, vec_t *points);



int testBBoxPInFrustumCheckWhole(viewport_t *v, vec_t *points);
int testSphereInFrustumNearPlane(viewport_t *v, vec_t *p, float size);
int testBBoxPInFrustumNearPlane(viewport_t *v, vec_t *points);

int resizeViewport(viewport_t *v, int width, int height);
#endif
