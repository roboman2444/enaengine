#ifndef ENTITYMANAGERHEADER
#define ENTITYMANAGERHEADER

struct entity_s;
struct entitylistpoint_s;
struct entitylistint_s;

typedef struct entity_s {
	char * name;
	int myid;
	char type;
	char flags;
	unsigned char treedepth;
	vec3_t pos;
	vec3_t angle;
	vec3_t anglevel;
	vec3_t vel;
	float scale;
	int needsmatupdate;
	matrix4x4_t mat;
	double nextthink;
	void (*spawn)(struct entity_s * e);
	void (*remove)(struct entity_s * e);
	void (*think)(struct entity_s * e);
	void (*touch)(struct entity_s *e, int touchid); //should be entity_t * toucher maybe
	int parentid;
	int attachmentid;

	int shaderid;		//frist priority for grouping
	int shaderperm;
	int texturegroupid;	//second priority
	int modelid;		//third priority

	int needsbboxupdate;

	vec_t bboxp[24];
	vec_t bbox[6];

	unsigned int leafpos;
	void * leaf;
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
entity_t * entity_findByNameRPOINT(const char * nam);
int entity_findByNameRINT(const char * name);
//returns a list of all entities with name
entitylistpoint_t entity_findAllByNameRPOINT(const char * name);
entitylistint_t entity_findAllByNameRINT(const char * name);
//returns pointer by entity id
entity_t * entity_returnById(const int id);
//creates and adds entity to listwith name
entity_t * entity_addRPOINT(const char * name);
int entity_addRINT(const char * name);
void entity_pruneList(void);

//deletes entity, only id is accepted
int entity_delete(const int id);
#endif
