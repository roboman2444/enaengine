#ifndef ENTITYMANAGERHEADER
#define ENTITYMANAGERHEADER

typedef struct entity_s {
	char * name;
	char type;
	vec3_t pos;
	vec3_t angle;
	float scale;
	matrix4x4_t mat;
	double nextthink;
	void (*spawn)(void);
	void (*remove)(void);
	void (*think)(void);
	void (*touch)(void * toucher); //should be entity_t * toucher
	model_t * model;
	texturegroup_t * texturegroup;
} entity_t;

entity_t *defaultentity;
int initEntitySystem(void);
int fbnumber;
int entitiesOK;

entity_t * findEntityByName(char * name);
entity_t * returnEntity(int it);
entity_t createEntity(char * name);
entity_t * addEntityToList(entity_t fb);
#endif
