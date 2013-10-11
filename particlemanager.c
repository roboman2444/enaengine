#include "globaldefs.h"
#include "particlemanager.h"

//#include "texturemanager.h" //todo
particlesystem_t *particlesyslist;
#define clearpartlist(point, num) if(point) memset(point, 0, num*sizeof(particle_t))
#define clearsyslist(num) if(particlesyslist) memset(particlesyslist, 0, num*sizeof(particlesystem_t))

#define MAXJUMPLEVEL 10


/* particle math


*/
//TODO

extern int delParticle(int lid, int id);
int partPhysics(int lid, float timescale){
	int i = particlesyslist[lid].firstOpenSlot;
	int end = particlesyslist[lid].topOfList + 1;
	for(; i < end; i++){
		if(particlesyslist[lid].particlelist[i].type){ //maybe turn into if(! )break
			particlesyslist[lid].particlelist[i].vel[0] += particlesyslist[lid].particlelist[i].gravity[0]*timescale;
			particlesyslist[lid].particlelist[i].vel[1] += particlesyslist[lid].particlelist[i].gravity[1]*timescale;
			particlesyslist[lid].particlelist[i].vel[2] += particlesyslist[lid].particlelist[i].gravity[2]*timescale;
//todo friction

			particlesyslist[lid].particlelist[i].pos[0] += particlesyslist[lid].particlelist[i].vel[0]*timescale;
			particlesyslist[lid].particlelist[i].pos[1] += particlesyslist[lid].particlelist[i].vel[1]*timescale;
			particlesyslist[lid].particlelist[i].pos[2] += particlesyslist[lid].particlelist[i].vel[2]*timescale;

//			particlesyslist[lid].particlelist[i].life -= particlesyslist[lid].particlelist[i].fade*timescale;
//			if(life<0) delParticle(lid, i);
			if( (particlesyslist[lid].particlelist[i].life -= particlesyslist[lid].particlelist[i].fade*timescale) < 0)delParticle(lid, i);


		}
	}
	return TRUE; //todo errorcheck
}

/* particle management


*/
void resizePartList(int id, int count){
	particlesyslist[id].max = count;
	particlesyslist[id].particlelist = realloc(particlesyslist[id].particlelist, count * sizeof(particle_t));
}
int searchForOpenPart(int id){
	//todo optimize
	int temp = particlesyslist[id].firstOpenSlot;
	for(; particlesyslist[id].particlelist[temp].type && temp < particlesyslist[id].max; temp++); //todo optimize
	particlesyslist[id].firstOpenSlot = temp;
	return temp;
}
int searchForTopPart(int id){
	//todo optimize
	int temp = particlesyslist[id].topOfList;
	for(; particlesyslist[id].particlelist[temp].type && temp > 0; temp--); //todo optimize
	particlesyslist[id].topOfList = temp;
	return temp;
}
int delParticle(int lid, int pid){
	//todo set first
	particlesyslist[lid].particlelist[pid].type = 0;
	if(pid < particlesyslist[lid].firstOpenSlot) particlesyslist[lid].firstOpenSlot = pid;
	searchForTopPart(lid);
	particlesyslist[lid].particlecount--;
	return TRUE; //todo errorcheck
}
int addParticle(int lid, vec3_t pos, vec3_t gravity, vec3_t vel, float life, float fade, char type){
	int pid = searchForOpenPart(lid);
	if (pid == particlesyslist[lid].max) resizePartList(lid, particlesyslist[lid].max+MAXJUMPLEVEL);
	particlesyslist[lid].particlelist[pid].life = life;
	particlesyslist[lid].particlelist[pid].vel[0] 	  = vel[0];
	particlesyslist[lid].particlelist[pid].vel[1] 	  = vel[1];
	particlesyslist[lid].particlelist[pid].vel[2] 	  = vel[2];
	particlesyslist[lid].particlelist[pid].pos[0] 	  = pos[0];
	particlesyslist[lid].particlelist[pid].pos[1] 	  = pos[1];
	particlesyslist[lid].particlelist[pid].pos[2] 	  = pos[2];
	particlesyslist[lid].particlelist[pid].gravity[0] = gravity[0];
	particlesyslist[lid].particlelist[pid].gravity[1] = gravity[1];
	particlesyslist[lid].particlelist[pid].gravity[2] = gravity[2];
	particlesyslist[lid].particlelist[pid].fade = fade;
	particlesyslist[lid].particlelist[pid].type = type;
	particlesyslist[lid].particlecount++; // maybe adda  check if it isnt type 0
	return TRUE;
	//todo debugging modes?
}





/* particle system management


*/

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

int searchForOpenSys(/*int start, int end*/){
	//todo start at firstopen slot, go until top of list or an type 0... wait 1+topoflist should technically be open
//	for(; particlesyslist[start].active && start < end; start++);
//	return start;
	for(; particlesyslist[firstOpenSysList].type && firstOpenSysList < maxSystems; firstOpenSysList++);
	return firstOpenSysList;
}
void resizeSysList(int count){
	maxSystems = count;
	particlesyslist = realloc(particlesyslist, maxSystems * sizeof(particlesystem_t));
}
//todo do a search for open system
int addParticleSys(char * name, vec3_t spawnpos, float lifespan, char type, int max){
	int id = searchForOpenSys();
	if (id == maxSystems) resizeSysList(maxSystems+MAXJUMPLEVEL);
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
		if(topOfSysList<id) topOfSysList = id;
		return id;
	}
	//todo work on this
	return FALSE; //eh
	//todo debugging modes?
}
int searchForTopSys(){
	for(; !particlesyslist[topOfSysList].type && topOfSysList > 0; topOfSysList--);
	return topOfSysList;
}

int delParticleSys(int id){
	//todo set first
	particlesyslist[id].type = 0;
	free(particlesyslist[id].name); //WARNING, do i want to do this?
	free(particlesyslist[id].particlelist);
	if(id < firstOpenSysList) firstOpenSysList = id;
	searchForTopSys();
	return TRUE; //todo errorcheck
}
