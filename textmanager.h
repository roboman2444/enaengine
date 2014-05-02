#ifndef TEXTMANAGERHEADER
#define TEXTMANAGERHEADER

typedef struct text_s{
	char type;
	char style;
	int myid;
	int numchars;
	GLuint textureid;
	GLuint height;
	GLuint width;
//	texture_t * texture;
	char color[3];
	char * name;
}text_t;

typedef struct font_s{
	char type;
//	texture_t * texture;
	char * filename;
	int myid;
	unsigned short size;
	//void because i dont want to #include ttf in other files
	void * font;
//	unsigned int size; //todo
	//todo stylings
//	TTF_Font * font;
}font_t;

typedef struct fontlistpoint_s {
	font_t **list;
	unsigned int count;
} fontlistpoint_t;
typedef struct fontlistint_s {
	int *list;
	unsigned int count;
} fontlistint_t;

typedef struct textlistpoint_s {
	text_t **list;
	unsigned int count;
} textlistpoint_t;
typedef struct textlistint_s {
	int *list;
	unsigned int count;
} textlistint_t;


//maybe enum these?
#define TEXT_FORMAT_ALPHA	1
#define TEXT_FORMAT_BOLD	2
#define TEXT_FORMAT_ITALIC	4
#define	TEXT_FORMAT_UNDERLINE	8

int textOK;
int textnumber;
int fontnumber;

text_t *textlist;
font_t *fontlist;
int textcount;
int textArraySize;
int textArrayLastTaken;
int fontcount;
int fontArraySize;
int fontArrayLastTaken;


int initTextSystem(void);

text_t createAndRenderText(char * name, int font, char style, char fgcolor[3]);
text_t createAndRenderTextFindFont(char * name, char * fontname, unsigned short size, char style, char fgcolor[3]);
font_t createAndLoadFont(char * name, unsigned short size);

text_t * addTextRPOINT(text_t text);
int addTextRINT(text_t text);
font_t * addFontRPOINT(font_t font);
int addFontRINT(font_t font);


text_t * findTextByNameRPOINT(char * name);
int findTextByNameRINT(char * name);

textlistpoint_t findTextsByNameRPOINT(char * name);
textlistint_t findTextsByNameRINT(char * name);

font_t * findFontByNameRPOINT(char * name);
int findFontByNameRINT(char * name);

fontlistpoint_t findFontsByNameRPOINT(char * name);
fontlistint_t findFontsByNameRINT(char * name);

int deleteText(int id);
int deleteFont(int id);

//text_t loadText(char *filepath, char type);

text_t * createAndAddTextFindFontRPOINT(char * name, char * fontname, unsigned short size, char style, char fgcolor[3]);
int createAndAddTextFindFontRINT(char * name, char * fontname, unsigned short size, char style, char fgcolor[3]);
text_t * createAndAddTextRPOINT(char * name, int font, char style, char fgcolor[3]);
int createAndAddTextRINT(char * name, int font, char style, char fgcolor[3]);

font_t * createAndAddFontRPOINT(char * name, unsigned short size);
int createAndAddFontRINT(char * name, unsigned short size);

text_t * returnTextById(int id);
font_t * returnFontById(int id);


int deleteAllText(void);
int deleteAllFont(void);


#endif

