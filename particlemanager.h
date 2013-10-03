#ifndef PARTICLEMANAGERHEADER
#define PARTICLEMANAGERHEADER

typedef struct particle_s {
	vec3_t vel;
	vec3_t pos;
	vec3_t gravity;
	float life;
	float friction;
	float fade;
	char type;
} particle_t;

typedef struct particlesystem_s {
	vec3_t spawnpos; //todo maybe not
	float lifespan;
	particle_t * particlelist;
	int particlecount;
	char type;
} particlesystem_t;

int particleSystemInit(int maxSystems);

#endif
