#ifndef ANIMMANAGERHEADER
#define ANIMMANAGERHEADER

typedef struct animscene_s {
	char * name;
	unsigned int firstframe;
	unsigned int framecount;
	float framerate;
	char loop; // also flags
}animscene_t;


typedef struct anim_s{
	unsigned int numscenes;
	animscene_t * scenelist;
	unsigned int numposes;
	float * posedata; // float because i dont think i need that much res... heck, even a 16bitf would work
	char type;
	char * name;
	int myid;
}anim_t;

int anim_ok;
int anim_groupnumber;

anim_t *anim_list;
int anim_count;
int anim_arraysize;
int anim_arraylasttaken;

anim_t anim_create(char * name);

anim_t * anim_addRPOINT(anim_t texgroup);
int anim_addRINT(anim_t texgroup);

int anim_init(void);

anim_t * anim_findByNameRPOINT(char * name);
int anim_findByNameRINT(char * name);

int anim_delete(int id);


//int loadiqmposes(anim_t *a, const struct iqmheader hdr, unsigned char *buf);
//int loadiqmanimscenes(anim_t *a, const struct iqmheader hdr, unsigned char *buf);

//anim_t createAndLoadAnim(char *name);
void anim_pruneList(void);


anim_t * anim_createAndAddRPOINT(char * name);
int anim_createAndAddRINT(char * name);

anim_t * anim_returnById(int id);

#endif

