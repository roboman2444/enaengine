#ifndef GLMANAGERHEADER
#define GLMANAGERHEADER


int glInit(void);
int glMainDraw(void);
int glResizeViewports(int width, int height);

unsigned int screenHeight;
unsigned int screenWidth;
unsigned int maxMSAASamples;


void GL_PrintError(int errornumber, const char *filename, int linenumber);
#define CHECKGLERROR {if (TRUE){if (FALSE) console_printf("CHECKGLERROR at %s:%d\n", __FILE__, __LINE__);int checkglerror_errornumber = glGetError();if (checkglerror_errornumber) GL_PrintError(checkglerror_errornumber, __FILE__, __LINE__);}}
#endif
