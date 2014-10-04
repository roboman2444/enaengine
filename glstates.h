#ifndef GLSTATESHEADER
#define GLSTATESHEADER

#define STATESENABLEDEPTH 1
#define STATESENABLEBLEND 2
#define STATESENABLECULLFACE 4
#define STATESENABLEMULTISAMPLE 8
#define STATESENABLEALPHATEST 16
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
} glstate_t;

void states_blendFunc(const GLenum source, const GLenum dest);
void states_alphaFunc(const GLenum func, const GLclampf ref);
void states_depthFunc(const GLenum dfunc);
void states_cullFace(const GLenum face);
void states_depthMask(const GLenum mask);

void states_bindVertexArray(const GLuint id);
void states_bindBuffer(const GLenum type, const GLuint id);
void states_bindBufferRange(const GLenum type, const GLuint index, const GLuint id, const GLintptr offset, const GLsizeiptr size);

void states_setState(glstate_t s);

void states_enableForce(GLenum en);
void states_enable(GLenum en);

void states_disableForce(GLenum en);
void states_disable(GLenum en);

#endif
