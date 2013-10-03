#include "globaldefs.h"
#include "particlemanager.h"

//#include "texturemanager.h" //todo

particlesystem_t *particlesyslist;

#define clearsyslist() if(particlesyslist) memset(particlesyslist, 0, maxSystems*sizeof(particlesystem_t))

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
