#ifndef CONSOLEHEADER
#define CONSOLEHEADER
int console_init(void); // works for now
int console_printf(const char *format, ...);
int console_nprintf(const size_t size, const char *format, ...);
int console_printBackwards(void);
int console_updateText(unsigned int offset, const unsigned int width, const unsigned int height);

//int consoleVAOid;

typedef struct consoleTextTracker_s {
	int textid;
	unsigned int textureid;
	char flag;
	unsigned int maxc;
	unsigned int count;
	float * verts;
	unsigned int * faces;
//	unsigned int height;
//	unsigned int width;
} consoleTextTracker_t;

unsigned int console_drawlines;
consoleTextTracker_t console_texttracker;
int console_displayneedsupdate;

int console_offset; //temp, will move
#endif
