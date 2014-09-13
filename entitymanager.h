#ifndef ENTITYMANAGERHEADER
#define ENTITYMANAGERHEADER

typedef struct entity_s {
	char * name;
	int myid;
	char type;
	vec3_t pos;
	vec3_t angle;
	vec3_t anglevel;
	vec3_t vel;
	float scale;
	int needsmatupdate;
	matrix4x4_t mat;
	double nextthink;
	void (*spawn)(void);
	void (*remove)(void);
	void (*think)(void);
	void (*touch)(int touchid); //should be entity_t * toucher
	int parentid;
	int attachmentid;

	int shaderid;		//frist priority for grouping
	int shaderperm;
	int texturegroupid;	//second priority
	int modelid;		//third priority

	int needsbboxupdate;

	vec_t bboxp[24];
	vec_t bbox[6];
	char flags;

//	vec3_t maxs; //worldspace bounding box
//	vec3_t mins;
} entity_t;

typedef struct entitylistpoint_s {
	//gotta use this stuff after you use it
	entity_t **list;
	unsigned int count;
} entitylistpoint_t; // yeah yeah i know naming

typedef struct entitylistint_s {
	//gotta use this stuff after you use it
	int *list;
	unsigned int count;
} entitylistint_t; // yeah yeah i know naming

entity_t *entity_list;
int entity_init(void);
int entity_count;
int entity_arraysize;
int entity_arraylasttaken;
int entity_ok;

//returns first entity it finds with name
entity_t * findEntityByNameRPOINT(char * nam);
int findEntityByNameRINT(char * name);
//returns a list of all entities with name
entitylistpoint_t findEntitiesByNameRPOINT(char * nam);
entitylistint_t findEntitiesByNameRINT(char * name);
//returns pointer by entity id
entity_t * returnEntityById(const int id);
//creates and adds entity to listwith name
entity_t * addEntityRPOINT(char * name);
int addEntityRINT(char * name);
void pruneEntityList(void);

//deletes entity, only id is accepted
int deleteEntity(const int id);
#endif
