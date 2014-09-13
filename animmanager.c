#include "globaldefs.h"
#include <string.h>

#include "hashtables.h"
#include "console.h"
#include "mathlib.h"
#include "matrixlib.h"
#include "animmanager.h"
#include "iqm.h"


int anim_ok = 0;
int anim_count = 0;
int anim_arrayfirstopen = 0;
int anim_arraylasttaken = -1;
int anim_arraysize = 0;

hashbucket_t animhashtable[MAXHASHBUCKETS];

anim_t *anim_list;

int anim_init(void){
	memset(animhashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(anim_list) free(anim_list);
	anim_list = 0;
//	anim_list = malloc(animnumber * sizeof(anim_t));
//	if(!anim_list) memset(anim_list, 0, animnumber * sizeof(anim_t));
//	defaultanim = addanimToList(createanim("default", 0));
	//todo error checking
	anim_ok= TRUE;
	return TRUE;
}

anim_t * anim_findByNameRPOINT(char * name){
	return anim_returnById(findByNameRINT(name, animhashtable));
}
int anim_findByNameRINT(char * name){
	return findByNameRINT(name, animhashtable);
}
int anim_delete(const int id){
	int animindex = (id & 0xFFFF);
	anim_t * a = &anim_list[animindex];
	if(a->myid != id) return FALSE;
	if(!a->name) return FALSE;
	deleteFromHashTable(a->name, id, animhashtable);
	free(a->name);
	memset(a,0, sizeof(anim_t));
	if(animindex < anim_arrayfirstopen) anim_arrayfirstopen = animindex;
	for(; anim_arraylasttaken > 0 && !anim_list[anim_arraylasttaken].type; anim_arraylasttaken--);
	return TRUE;
}
anim_t * anim_returnById(const int id){
	int animindex = (id & 0xFFFF);
	anim_t * a = &anim_list[animindex];
	if(!a->type) return FALSE;
	if(a->myid == id) return a;
	return FALSE;
}

anim_t anim_create(char * name){
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
int anim_addRINT(anim_t a){
	anim_count++;
	for(; anim_arrayfirstopen < anim_arraysize && anim_list[anim_arrayfirstopen].type; anim_arrayfirstopen++);
	if(anim_arrayfirstopen == anim_arraysize){	//resize
		anim_arraysize++;
		anim_list = realloc(anim_list, anim_arraysize * sizeof(anim_t));
	}
	anim_list[anim_arrayfirstopen] = a;
	int returnid = (anim_count << 16) | anim_arrayfirstopen;
	anim_list[anim_arrayfirstopen].myid = returnid;

	addToHashTable(anim_list[anim_arrayfirstopen].name, returnid, animhashtable);
	if(anim_arraylasttaken < anim_arrayfirstopen) anim_arraylasttaken = anim_arrayfirstopen; //todo redo
	return returnid;
}
anim_t * anim_addRPOINT(anim_t a){
	anim_count++;
	for(; anim_arrayfirstopen < anim_arraysize && anim_list[anim_arrayfirstopen].type; anim_arrayfirstopen++);
	if(anim_arrayfirstopen == anim_arraysize){	//resize
		anim_arraysize++;
		anim_list = realloc(anim_list, anim_arraysize * sizeof(anim_t));
	}
	anim_list[anim_arrayfirstopen] = a;
	int returnid = (anim_count << 16) | anim_arrayfirstopen;
	anim_list[anim_arrayfirstopen].myid = returnid;

	addToHashTable(anim_list[anim_arrayfirstopen].name, returnid, animhashtable);
	//todo maybe have anim have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(anim_arraylasttaken < anim_arrayfirstopen) anim_arraylasttaken = anim_arrayfirstopen;
//	printf("animarraysize = %i\n", anim_arraysize);
//	printf("anim_count = %i\n", anim_count);

	return &anim_list[anim_arrayfirstopen];

}
int anim_createAndAddRINT(char * name){
	int m = anim_findByNameRINT(name);
	if(m) return m;
	return anim_addRINT(anim_create(name));
//	return &anim_list[addanimToList(createAndLoadanim(name))];
}
anim_t * anim_createAndAddRPOINT(char * name){
	anim_t * m = anim_findByNameRPOINT(name);
	if(m) return m;
	return anim_addRPOINT(anim_create(name));
//	return &anim_list[addanimToList(createAndLoadanim(name))];
}
void anim_pruneList(void){
	if(anim_arraysize == anim_arraylasttaken+1) return;
	anim_arraysize = anim_arraylasttaken+1;
	anim_list = realloc(anim_list, anim_arraysize * sizeof(anim_t));
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
