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

int textureCurrentBound;

int animsOK;
int animgroupnumber;

anim_t *animlist;
int animcount;
int animArraySize;
int animArrayLastTaken;

anim_t createAnim(char * name);

anim_t * addAnimRPOINT(anim_t texgroup);
int addAnimRINT(anim_t texgroup);

int initAnimSystem(void);

anim_t * findAnimByNameRPOINT(char * name);
int findAnimByNameRINT(char * name);

int deleteAnim(int id);


//int loadiqmposes(anim_t *a, const struct iqmheader hdr, unsigned char *buf);
//int loadiqmanimscenes(anim_t *a, const struct iqmheader hdr, unsigned char *buf);

//anim_t createAndLoadAnim(char *name);
anim_t createAnim(char *name);


anim_t * createAndAddAnimRPOINT(char * name);
int createAndAddAnimRINT(char * name);

anim_t * returnAnimById(int id);

#endif

