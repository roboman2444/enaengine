#ifndef CONSOLEHEADER
#define CONSOLEHEADER
int initConsoleSystem(void); // works for now
int consolePrintf(const char *format, ...);
int consoleNPrintf(size_t size, const char *format, ...);
int printConsoleBackwards(void);
int updateConsoleVBO(void);

int consoleVAOid;
#endif
