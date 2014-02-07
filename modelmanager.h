#ifndef MODELMANAGERHEADER
#define MODELMANAGERHEADER

typedef struct model_s {
	char * name;
	texturegroup_t * texture;
	vbo_t * vbo;
	vec6_t bbox;
	char type; // maybe not
//	GLuint * numfaces;
//	GLuint numlod;
//	GLuint numverts;
}model_t;

/* types of models ... i should do enums for these

0 - unused/error
1 - static
2 - nonstatic, no bones
3 - nonstatic with bones // maybe sperate struct

*/

int modelsOK;
int modelnumber;

model_t * defaultModel;


model_t * addModelToList(model_t model);
int initModelSystem(void);
model_t * findModelByName(char * name);
model_t createAndLoadModel(char * name);
model_t createAndLoadTypeModel(char * name, char type);
model_t * createAndAddModel(char * name);
#endif

