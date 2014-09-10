#ifndef CONSOLEHEADER
#define CONSOLEHEADER
int console_init(void); // works for now
int console_printf(const char *format, ...);
int console_nprintf(size_t size, const char *format, ...);
int console_printBackwards(void);
int console_updateText(unsigned int offset);

//int consoleVAOid;

typedef struct consoleTextTracker_s {
	int textid;
	unsigned int textureid;
	char flag;
	unsigned int height;
	unsigned int width;
} consoleTextTracker_t;

unsigned int console_drawlines;
consoleTextTracker_t * console_texttracker;
int console_displayneedsupdate;

int console_offset; //temp, will move
#endif
