#ifndef MODELMANAGERHEADER
#define MODELMANAGERHEADER

typedef struct model_s {
	texturegroup_t * texture;
	GLuint vboid;// maybe not
}model_t;


/* types of models ... i should do enums for these

0 - unused/error
1 - static
2 - nonstatic, no bones
3 - nonstatic with bones // maybe sperate struct

*/
#endif

