#ifndef ENTITYMANAGERHEADER
#define ENTITYMANAGERHEADER

typedef struct entity_s {
	char * name;
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
	model_t * model;
	texturegroup_t * texturegroup;
} entity_t;

entity_t *defaultentity;
entity_t **entitylist;
int initEntitySystem(void);
int entitynumber;
int entitiesOK;

entity_t * findEntityByName(char * name);
entity_t * returnEntity(int it);
entity_t createEntity(char * name);
entity_t * addEntityToList(entity_t fb);
#endif
