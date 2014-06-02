#ifndef ANIMMANAGERHEADER
#define ANIMMANAGERHEADER

typedef struct anim_s{
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

anim_t createAndLoadAnim(char *name);


anim_t * createAndAddAnimRPOINT(char * name);
int createAndAddAnimRINT(char * name);

anim_t * returnAnimById(int id);

#endif

