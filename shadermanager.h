#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER

typedef struct shader_s
{
	GLuint id;
	char * name;
	char type; // 0 is vert, 1 is frag SO FAR... may change it up if i slap geo and the misc tess stuff
} shader_t;
typedef struct shaderprogram_s
{
	GLuint id;
	char * name;
	shader_t vertex;
	shader_t fragment;
} shaderprogram_t;
int initShaderSystem(void);
int createAndLoadShader(const char * name, char type);
#endif
