#ifndef GLSTATESHEADER
#define GLSTATESHEADER

#define STATESENABLEDEPTH 1
#define STATESENABLEBLEND 2
#define STATESENABLECULLFACE 4
#define STATESENABLEMULTISAMPLE 8
#define STATESENABLEALPHATEST 16

void statesBlendFunc(const GLenum source, const GLenum dest);
void statesDepthFunc(const GLenum dfunc);
void statesCullFace(const GLenum face);
void statesDepthMask(const GLenum mask);

void statesEnableForce(GLenum en);
void statesEnable(GLenum en);

void statesDisableForce(GLenum en);
void statesDisable(GLenum en);

#endif
