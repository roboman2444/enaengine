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

/*
	particlesystem type;
	0 is dead/inactive
	1 is addative, no sorting.
	//should make a bitfield


*/

int particleSysCount = 0;

int particleSystemInit(int maxSystems);
int addParticleSys(char * name, vec3_t spawnpos, float lifespan, char type, int max);
int delParticleSys(int id);
int addParticle(int lid, vec3_t pos, vec3_t gravity, vec3_t vel, float life, float fade, char type);

#endif
