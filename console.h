#ifndef CONSOLEHEADER
#define CONSOLEHEADER
int initConsoleSystem(void); // works for now
int consolePrintf(const char *format, ...);
int consoleNPrintf(size_t size, const char *format, ...);
int printConsoleBackwards(void);
int updateConsoleText(void);

//int consoleVAOid;

typedef struct consoleTextTracker_s {
	int textid;
	unsigned int textureid;
	char flag;
	unsigned int height;
	unsigned int width;
} consoleTextTracker_t;

unsigned int consoleDrawLines;
consoleTextTracker_t * texttracker;
int consoleDisplayNeedsUpdate;
#endif
