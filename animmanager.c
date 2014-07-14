#include "globaldefs.h"
#include <string.h>

#include "hashtables.h"
#include "console.h"
#include "mathlib.h"
#include "matrixlib.h"
#include "animmanager.h"
#include "iqm.h"


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
/*
anim_t createAndLoadAnim(char * name){
	anim_t a;
	memset(&a, 0, sizeof(anim_t));
	//todo clean up anim if it already has shit in it.
	a.name = malloc(strlen(name)+1);
	strcpy(a.name, name);
	a.type = 1;
	return a;
}
*/
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
	return addAnimRINT(createAnim(name));
//	return &animlist[addanimToList(createAndLoadanim(name))];
}
anim_t * createAndAddAnimRPOINT(char * name){
	anim_t * m = findAnimByNameRPOINT(name);
	if(m) return m;
	return addAnimRPOINT(createAnim(name));
//	return &animlist[addanimToList(createAndLoadanim(name))];
}
void pruneAnimList(void){
	if(animArraySize == animArrayLastTaken+1) return;
	animArraySize = animArrayLastTaken+1;
	animlist = realloc(animlist, animArraySize * sizeof(anim_t));
}

int loadiqmposes(anim_t *a, const struct iqmheader hdr, unsigned char *buf){
	int i, k, j;

	//todo have a error return
	a->numposes = hdr.num_poses;
	a->posedata = malloc(7*hdr.num_poses*sizeof(float));
	float *posedata = a->posedata;
	struct iqmpose *poses = (struct iqmpose *)&buf[hdr.ofs_poses];
	unsigned short * framedata = (unsigned short *)&buf[hdr.ofs_frames];
	for(i = 0, k = 0; i < hdr.num_frames; i++){
		for( j =0; j < hdr.num_poses; j++, k++){
			struct iqmpose *p = &poses[j];
			float rot[4];
			posedata[k*7+0] = p->channeloffset[0]; if(p->mask&1) posedata[k*7+0] +=*framedata++ * p->channelscale[0];
			posedata[k*7+1] = p->channeloffset[1]; if(p->mask&2) posedata[k*7+1] +=*framedata++ * p->channelscale[1];
			posedata[k*7+2] = p->channeloffset[2]; if(p->mask&4) posedata[k*7+2] +=*framedata++ * p->channelscale[2];
			rot[0] = p->channeloffset[3]; if(p->mask&8)  rot[0] +=*framedata++ * p->channelscale[3];
			rot[1] = p->channeloffset[4]; if(p->mask&16) rot[1] +=*framedata++ * p->channelscale[4];
			rot[2] = p->channeloffset[5]; if(p->mask&32) rot[2] +=*framedata++ * p->channelscale[5];
			rot[3] = p->channeloffset[6]; if(p->mask&64) rot[3] +=*framedata++ * p->channelscale[6];
			if(rot[3] > 0) vec4mult(rot, rot, -1.0);
			vec4norm2(rot, rot);
			posedata[k*7+3] = 32767.0f * rot[0];
			posedata[k*7+4] = 32767.0f * rot[1];
			posedata[k*7+5] = 32767.0f * rot[2];
			posedata[k*7+6] = 32767.0f * rot[3];

			//skip scale data
			if(p->mask&128)framedata++;
			if(p->mask&256)framedata++;
			if(p->mask&512)framedata++;
		}
	}
	//no default for 0 poses



	return hdr.num_poses;
}


int loadiqmanimscenes(anim_t * a, const struct iqmheader hdr, unsigned char *buf){

	if(hdr.num_anims < 1){
		a->numscenes = 1;
		a->scenelist = malloc(1 * sizeof(animscene_t));
		animscene_t *ascene = a->scenelist;
		ascene->name = malloc(strlen("static")+1); //todo maybe redo this
		strcpy(ascene->name, "static");
		ascene->firstframe = 0;
		ascene->framecount = 1;
		ascene->loop = TRUE;
		ascene->framerate = 10.0;
		return TRUE;
	}


	//todo have an error return
	int i;
	a->numscenes = hdr.num_anims;
	a->scenelist = malloc(hdr.num_anims * sizeof(animscene_t));
	struct iqmanim * anims = (struct iqmanim *)(buf+hdr.ofs_anims);
	const char * text = hdr.num_text && hdr.ofs_text ? (const char *)(buf + hdr.ofs_text) : "";
	for(i = 0; i < hdr.num_anims; i++){
		animscene_t *ascene = &a->scenelist[i];
		struct iqmanim * anim = &anims[i];
		ascene->name = malloc(strlen(&text[anim->name])+1); //todo maybe redo this
		strcpy(ascene->name, &text[anim->name]);
		ascene->firstframe = anim->first_frame;
		ascene->framecount = anim->num_frames;
		ascene->loop = ((anim->flags & IQM_LOOP)!= 0);
		ascene->framerate = anim->framerate;
	}
	//todo default?
	return TRUE;
}
