#ifndef RENDERQUEUEHEADER
#define RENDERQUEUEHEADER

typedef struct modelbatche_s {
	int modelid;
	int count;
	matrix4x4_t * matlist;
//	matrix4x4_t * cammatlist;
} modelbatche_t;
typedef struct texturebatche_s {
	int count;
	modelbatche_t * modelbatch;
//	struct texturebatch_s * next; //im lazy and would rather do arrays
	int textureid;
} texturebatche_t;
typedef struct shaderbatche_s {
	int count;
	texturebatche_t * texturebatch;
	int shaderid;
	int shaderperm;
} shaderbatche_t;
typedef struct renderbatche_s {
	int count;
	shaderbatche_t * shaderbatch;
	//int something;
}renderbatche_t; // the e stands for entity





typedef struct rendervertdata_s {
	GLfloat * posvertdata;
	GLfloat * normvertdata;
	GLfloat * tcvertdata;
	GLfloat * tangentvertdata;
	GLfloat * blendivertdata;
	GLfloat * blendwvertdata;
	GLuint * facedata;
	GLuint numfaces;
	GLuint numverts;
	GLuint numtimes;
//	renderuniformdata_t * udata; //todo maybe change to id system?
	int textureid;
	int shaderid;
	int shaderperm;
} rendervertdata_t;

typedef struct rendervbodata_s {
	int vboid;
	GLuint start;
	GLuint numfaces;
	GLuint numtimes;
//	renderuniformdata_t * udata; //todo maybe change to id system?
	int textureid;
	int shaderid;
	int shaderperm;
} rendervbodata_t;

typedef enum renderlisttype_e {
	RENDERENT,
	RENDERVERTDATA,
	RENDERVBO
//	SORT
} renderlisttype;

typedef struct renderlistnode_s {
	void * data;
	struct renderlistnode_s *next;
	struct renderlistnode_s *previous;
	renderlisttype type;
} renderlistnode_t;

typedef struct renderlist_s {
	unsigned int count;
	renderlisttype * types;
	void ** renderlist;
//	renderlistnode_t * start;
//	renderlistnode_t * end;
} renderlist_t;
typedef struct rendervboupqueue_s {
	unsigned int count;
	rendervbodata_t * list;
} rendervboupqueue_s;
/*
typedef struct lightbatche_s {
	int count;
	light_t * lightlist;
} lightbatche_t;
*/
int addEntityToRenderbatche(entity_t * ent, renderbatche_t * batch);
int addObjectToRenderbatche(worldobject_t * obj, renderbatche_t * batch);

int cleanupRenderbatche(renderbatche_t * batch);


//void addLightToLightbatche(light_t * l, lightbatche_t * batch);
//void cleanupLightbatche(lightbatche_t * batch);

#endif
