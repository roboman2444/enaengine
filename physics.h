#ifndef PHYSICSHEADER
#define PHYSICSHEADER


typedef struct phystype_s {
	enum {STATIC, FLY, DYNAMIC, ODEFLY, ODEDYNAMIC} movetype;
	enum {NONE, AABB, ODESPHERE, ODEBOX, ODECYLINDER, ODECAPSULE} collidetype;
	float mass;
	void * gid;
	void * bid;
} phystype_t;


int physics_ok;


int physics_init(void);
int physics_destroy(void);

//void physics_nearCollide(void * data, void * gid1, void * gid2);
void physicsnearCollideFCall(phystype_t ph1, phystype_t ph2);
void physics_odeTick(float time);
int physics_getEntD(void *e);
#endif
