#ifndef PARTICLEMANAGERHEADER
#define PARTICLEMANAGERHEADER

typedef struct particle_s {
	vec3_t vel;
	vec3_t pos;
	vec3_t gravity;
	float life;
	float friction;
	float fade;
	char type; //0 is inactive/dead
} particle_t;

typedef struct particlesystem_s {
	vec3_t spawnpos; //todo maybe not
	float lifespan;
	particle_t * particlelist;
	char * name; //just because reasons
	int particlecount;
	int max;
	int firstOpenSlot;
	int topOfList;
	char type; //0 is inactive/dead
} particlesystem_t;

int particleSysCount = 0;

int particleSystemInit(int maxSystems);

#endif
