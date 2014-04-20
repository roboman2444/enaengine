#ifndef TEXTMANAGERHEADER
#define TEXTMANAGERHEADER

typedef struct text_s{
	char type;
	int numchars;
	GLuint textureid;
	GLuint height;
	GLuint width;
//	texture_t * texture;
	char * name;
	int myid;
}text_t;

typedef struct font_s{
	char type;
//	texture_t * texture;
	char * filename;
	int myid;
	//void because i dont want to #include ttf in other files
	void * font;
//	unsigned int size; //todo
	//todo stylings
//	TTF_Font * font;
}font_t;

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

text_t createAndRenderText(char * name, char * fontname);
font_t createAndLoadFont(char * name);

text_t * addTextRPOINT(text_t text);
int addTextRINT(text_t text);
font_t * addFontRPOINT(font_t font);
int addFontRINT(font_t font);


text_t * findTextByNameRPOINT(char * name);
int findTextByNameRINT(char * name);

font_t * findFontByNameRPOINT(char * name);
int findFontByNameRINT(char * name);

int deleteText(int id);
int deleteFont(int id);

//text_t loadText(char *filepath, char type);

text_t * createAndAddTextRPOINT(char * name, char * fontname);
int createAndAddTextRINT(char * name, char * fontname);
font_t * createAndAddFontRPOINT(char * name);
int createAndAddFontRINT(char * name);

text_t * returnTextById(int id);
font_t * returnFontById(int id);


int deleteAllText(void);
int deleteAllFont(void);


#endif

