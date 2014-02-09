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
	model_t * model;
	texturegroup_t * texturegroup;
} entity_t;

typedef struct entitylistpoint_s {
	entity_t **list;
	int count;
} entitylistpoint_t; // yeah yeah i know naming

typedef struct entitylistint_s {
	int *list;
	int count;
} entitylistint_t; // yeah yeah i know naming

entity_t *entitylist;
int initEntitySystem(void);
int entitycount;
int entityArraySize;
int entityArrayLastTaken;
int entitiesOK;

entity_t * findEntityByNameRPOINT(char * nam);
int findEntityByNameRINT(char * name);
entitylistpoint_t findEntitiesByNameRPOINT(char * nam);
entitylistint_t findEntitiesByNameRINT(char * name);
entity_t * returnById(int id);
//entity_t createEntity(char * name); // not supposed to do this yourself
entity_t * addEntityRPOINT(char * name);
int addEntityRINT(char * name);
int deleteEntity(int id);
#endif
