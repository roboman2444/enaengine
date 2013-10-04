#include "globaldefs.h"
#include "particlemanager.h"

//#include "texturemanager.h" //todo

particlesystem_t *particlesyslist;

#define clearsyslist() if(particlesyslist) memset(particlesyslist, 0, maxSystems*sizeof(particlesystem_t))

#define MAXJUMPLEVEL 10

int maxSystems;

int particleSystemInit(int max){
	if(particlesyslist) free(particlesyslist);
	if( ( particlesyslist = malloc(max*sizeof(particlesystem_t)) ) ){ //should work..
		maxSystems = max;
		clearsyslist();
		return TRUE;
	}
	return FALSE;
}
//todo do a search for open system
int addParticleSys(int id, char * name, vec3_t spawnpos, float lifespan, char type, int max){
	particlesyslist[id].name = name;
//	particlesyslist[id].spawnpos = spawnpos;
	particlesyslist[id].lifespan = lifespan;
	particlesyslist[id].type = type;
	particlesyslist[id].max = max;
	return TRUE; //eh
}
