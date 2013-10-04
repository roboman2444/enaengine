#include "globaldefs.h"
#include "particlemanager.h"

//#include "texturemanager.h" //todo

particlesystem_t *particlesyslist;

#define clearsyslist(num) if(particlesyslist) memset(particlesyslist, 0, num*sizeof(particlesystem_t))
#define clearpartlist(point, num) if(point) memset(point, 0, num*sizeof(particle_t))

#define MAXJUMPLEVEL 10

int maxSystems;

int topOfSysList=0;

int firstOpenSysList=0;

int particleSystemInit(int max){
	if(particlesyslist) free(particlesyslist);
	if( ( particlesyslist = malloc(max*sizeof(particlesystem_t)) ) ){ //should work.. //maybe change to calloc
		maxSystems = max;
		clearsyslist(maxSystems);
		return TRUE;
	}
	return FALSE;
}

int searchForOpen(void){
	//todo start at firstopen slot, go until top of list or an type 0... wait 1+topoflist should technically be open
	return TRUE; //REMOVE THIS
}
//todo do a search for open system
int addParticleSys(int id, char * name, vec3_t spawnpos, float lifespan, char type, int max){
	particlesyslist[id].particlecount = 0;
	particlesyslist[id].name = name;
	particlesyslist[id].spawnpos[0] = spawnpos[0];
	particlesyslist[id].spawnpos[1] = spawnpos[1];
	particlesyslist[id].spawnpos[2] = spawnpos[2];
	particlesyslist[id].lifespan = lifespan;
	particlesyslist[id].type = type;
	particlesyslist[id].max = max;
	//maybe different way...
	if( ( particlesyslist[id].particlelist = malloc(max*sizeof(particle_t)) ) ){ //maybe change to calloc
		clearpartlist(particlesyslist[id].particlelist, max);
		return TRUE;
	}
	//todo work on this
	return FALSE; //eh
}
int delParticleSys(int id){
	//todo set first
	particlesyslist[id].type = 0;
	free(particlesyslist[id].name); //WARNING, do i want to do this?
	free(particlesyslist[id].particlelist);

	return TRUE; //todo errorcheck
}
