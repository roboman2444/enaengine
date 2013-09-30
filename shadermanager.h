#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER

typedef struct shaderprogram_s
{
	GLuint id;
	char * name;
	GLuint vertexid;
	GLuint fragmentid;
} shaderprogram_t;
int initShaderSystem(void);
int createAndLoadShader(char * name);
#endif
