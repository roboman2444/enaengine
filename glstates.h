#ifndef GLSTATESHEADER
#define GLSTATESHEADER

#define STATESENABLEDEPTH 1
#define STATESENABLEBLEND 2
#define STATESENABLECULLFACE 4
#define STATESENABLEMULTISAMPLE 8
#define STATESENABLEALPHATEST 16


#define STATESTEXTUREUNITCOUNT 8
#define STATESTENABLEDTRACK char
//todo add more

typedef struct glstate_s {
	char enabledstates;

	GLenum blendsource;
	GLenum blenddest;

	GLenum depthfunc;

	GLenum cullface;

	GLenum depthmask;

	GLenum alphafunc;
	GLclampf alpharef;

	GLuint vaoid;

	GLuint vboid;
	GLenum vbotype;
	GLuint vborangei;
	GLintptr vborangeo;
	GLsizeiptr vboranges;

	GLuint shaderid;

//	GLenum activetexture;
	unsigned char activetexture;
	 //IF I CHANGE STATESTEXTUREUNITCOUNT, I HAVE TO MAKE THIS BIGGER
	STATESTENABLEDTRACK enabledtextures; //only used when applying a whole state
	GLuint textureunitid[STATESTEXTUREUNITCOUNT];
	GLenum textureunittarget[STATESTEXTUREUNITCOUNT];
} glstate_t;

void states_blendFunc(const GLenum source, const GLenum dest);
void states_alphaFunc(const GLenum func, const GLclampf ref);
void states_depthFunc(const GLenum dfunc);
void states_cullFace(const GLenum face);
void states_depthMask(const GLenum mask);

void states_bindVertexArray(const GLuint id);
void states_bindBuffer(const GLenum type, const GLuint id);
void states_bindBufferRange(const GLenum type, const GLuint index, const GLuint id, const GLintptr offset, const GLsizeiptr size);

char states_useProgram(const GLuint shaderid);

//void states_activeTexture(const GLenum activetexture);
void states_activeTexture(const unsigned char activetexture);

void states_bindTexture(const GLenum target, const GLuint id);
void states_bindActiveTexture(const unsigned char activetexture, const GLenum target, const GLuint id);

void states_setState(const glstate_t s);

void states_enableForce(const GLenum en);
void states_enable(const GLenum en);

void states_disableForce(const GLenum en);
void states_disable(const GLenum en);

#endif
