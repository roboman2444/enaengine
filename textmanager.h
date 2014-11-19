#ifndef TEXTMANAGERHEADER
#define TEXTMANAGERHEADER
/*
typedef struct text_s{
	char type;
	char style;
	int myid;
	int numchars;
	GLuint textureid;
	GLuint width;
	GLuint height;
	float fontsize;
//	texture_t * texture;
	char * name;
}text_t;
*/
typedef struct font_s{
	char type;
	char * filename;
	int myid;
	size_t datasize;
	//void because i dont want to #include ttf in other files
	void * ttfdata;
	GLuint width;
	GLuint height;
	void * cdata;
	GLuint textureid;
	float fontsize;
	//todo stylings
}font_t;

typedef struct fontlistpoint_s {
	font_t **list;
	unsigned int count;
} fontlistpoint_t;
typedef struct fontlistint_s {
	int *list;
	unsigned int count;
} fontlistint_t;
/*
typedef struct textlistpoint_s {
	text_t **list;
	unsigned int count;
} textlistpoint_t;
typedef struct textlistint_s {
	int *list;
	unsigned int count;
} textlistint_t;
*/

//maybe enum these?
#define TEXT_FORMAT_ALPHA	1
#define TEXT_FORMAT_BOLD	2
#define TEXT_FORMAT_ITALIC	4
#define	TEXT_FORMAT_UNDERLINE	8

int text_ok;
int textnumber;
int fontnumber;

//text_t *textlist;
font_t *fontlist;
//int textcount;
//int textArraySize;
//int textArrayLastTaken;
int fontcount;
int fontArraySize;
int fontArrayLastTaken;


int text_init(void);

//text_t createAndRenderText(const char * name, const int font, const unsigned int x, const unsigned int y, const float fontsize);
//text_t createAndRenderTextFindFont(const char * name, const char * fontname, const unsigned int x, const unsigned int y, const float fontsize);
font_t createAndLoadFont(const char * name);
/*
text_t * addTextRPOINT(text_t text);
int addTextRINT(text_t text);
font_t * addFontRPOINT(font_t font);
int addFontRINT(font_t font);


text_t * findTextByNameRPOINT(const char * name);
int findTextByNameRINT(const char * name);

textlistpoint_t findTextsByNameRPOINT(const char * name);
textlistint_t findTextsByNameRINT(const char * name);
*/
font_t * findFontByNameRPOINT(const char * name);
int findFontByNameRINT(const char * name);

fontlistpoint_t findFontsByNameRPOINT(const char * name);
fontlistint_t findFontsByNameRINT(const char * name);

//int deleteText(const int id);
int deleteFont(const int id);

//text_t loadText(char *filepath, char type);
/*
text_t * createAndAddTextFindFontRPOINT(const char * name, const char * fontname, const unsigned int x, const unsigned int y, const float fontsize);
int createAndAddTextFindFontRINT(const char * name, const char * fontname, const unsigned int x, const unsigned int y, const float fontsize);
text_t * createAndAddTextRPOINT(const char * name, const int font, const unsigned int x, const unsigned int y, const float fontsize);
int createAndAddTextRINT(const char * name, const int font, const unsigned int x, const unsigned int y, const float fontsize);
*/
font_t * createAndAddFontRPOINT(const char * name);
int createAndAddFontRINT(const char * name);

//text_t * returnTextById(const int id);
font_t * returnFontById(const int id);


//int deleteAllText(void);
int deleteAllFont(void);


#endif

