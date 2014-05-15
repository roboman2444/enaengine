#ifndef PARTICLEMANAGERHEADER
#define PARTICLEMANAGERHEADER

/*typedef struct particle_s {
	vec3_t vel;
	vec3_t pos;
	vec3_t gravity; // should be per particle sys todo
	float life;
	float friction; //should be per particle sys todo
	float fade;
	char type; //0 is inactive/dead
} particle_t;

typedef struct particlesystem_s {
	//todo have a callback for spawning function... and the physic function
	vec3_t spawnpos; //todo maybe not
	vec3_t gravity; // should be per particle sys
	float lifespan;
	float friction; //should be per particle sys
	particle_t * particlelist;
	char * name; //just because reasons
	int particlecount;
	int max;
	int firstOpenSlot;
	int topOfList;
	char type; //0 is inactive/dead


} particlesystem_t;


int particleSysCount;
int particlesOK;

int initParticleSystem(int maxSystems);
int addParticleSys(char * name, vec3_t spawnpos, float lifespan, char type, int max);
int delParticleSys(int id);
int addParticle(int lid, vec3_t pos, vec3_t gravity, vec3_t vel, float life, float fade, char type);
*/
#endif
