#include "globaldefs.h"

#include "hashtables.h"
#include "console.h"
#include "animmanager.h"


int animsOK = 0;
int animcount = 0;
int animArrayFirstOpen = 0;
int animArrayLastTaken = -1;
int animArraySize = 0;

hashbucket_t animhashtable[MAXHASHBUCKETS];

anim_t *animlist;

int initAnimSystem(void){
	memset(animhashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(animlist) free(animlist);
	animlist = 0;
//	animlist = malloc(animnumber * sizeof(anim_t));
//	if(!animlist) memset(animlist, 0, animnumber * sizeof(anim_t));
//	defaultanim = addanimToList(createanim("default", 0));
	//todo error checking
	animsOK = TRUE;
	return TRUE;
}

anim_t * findAnimByNameRPOINT(char * name){
	return returnAnimById(findByNameRINT(name, animhashtable));
}
int findAnimByNameRINT(char * name){
	return findByNameRINT(name, animhashtable);
}
int deleteAnim(int id){
	int animindex = (id & 0xFFFF);
	anim_t * a = &animlist[animindex];
	if(a->myid != id) return FALSE;
	if(!a->name) return FALSE;
	free(a->name);
	memset(a,0, sizeof(anim_t));
	if(animindex < animArrayFirstOpen) animArrayFirstOpen = animindex;
	for(; animArrayLastTaken > 0 && !animlist[animArrayLastTaken].type; animArrayLastTaken--);
	return TRUE;
}
anim_t * returnAnimById(int id){
	int animindex = (id & 0xFFFF);
	anim_t * a = &animlist[animindex];
	if(!a->type) return FALSE;
	if(a->myid == id) return a;
	return FALSE;
}

anim_t createAnim(char * name){
	anim_t a;
	a.name = malloc(strlen(name)+1);
	strcpy(a.name, name);
	return a;
}

anim_t createAndLoadAnim(char * name){
	anim_t a;
	memset(&a, 0, sizeof(anim_t));
	//todo clean up anim if it already has shit in it.
	a.name = malloc(strlen(name)+1);
	strcpy(a.name, name);
	a.type = 1;
	return a;
}

int addAnimRINT(anim_t a){
	animcount++;
	for(; animArrayFirstOpen < animArraySize && animlist[animArrayFirstOpen].type; animArrayFirstOpen++);
	if(animArrayFirstOpen == animArraySize){	//resize
		animArraySize++;
		animlist = realloc(animlist, animArraySize * sizeof(anim_t));
	}
	animlist[animArrayFirstOpen] = a;
	int returnid = (animcount << 16) | animArrayFirstOpen;
	animlist[animArrayFirstOpen].myid = returnid;

	addToHashTable(animlist[animArrayFirstOpen].name, returnid, animhashtable);
	if(animArrayLastTaken < animArrayFirstOpen) animArrayLastTaken = animArrayFirstOpen; //todo redo
	return returnid;
}
anim_t * addAnimRPOINT(anim_t a){
	animcount++;
	for(; animArrayFirstOpen < animArraySize && animlist[animArrayFirstOpen].type; animArrayFirstOpen++);
	if(animArrayFirstOpen == animArraySize){	//resize
		animArraySize++;
		animlist = realloc(animlist, animArraySize * sizeof(anim_t));
	}
	animlist[animArrayFirstOpen] = a;
	int returnid = (animcount << 16) | animArrayFirstOpen;
	animlist[animArrayFirstOpen].myid = returnid;

	addToHashTable(animlist[animArrayFirstOpen].name, returnid, animhashtable);
	//todo maybe have anim have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(animArrayLastTaken < animArrayFirstOpen) animArrayLastTaken = animArrayFirstOpen;
//	printf("animarraysize = %i\n", animArraySize);
//	printf("animcount = %i\n", animcount);

	return &animlist[animArrayFirstOpen];

}
int createAndAddAnimRINT(char * name){
	int m = findAnimByNameRINT(name);
	if(m) return m;
	return addAnimRINT(createAndLoadAnim(name));
//	return &animlist[addanimToList(createAndLoadanim(name))];
}
anim_t * createAndAddAnimRPOINT(char * name){
	anim_t * m = findAnimByNameRPOINT(name);
	if(m) return m;
	return addAnimRPOINT(createAndLoadAnim(name));
//	return &animlist[addanimToList(createAndLoadanim(name))];
}
