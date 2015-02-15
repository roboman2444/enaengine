#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include "glstates.h"
#include <sys/stat.h> //todo filesys


#include "globaldefs.h"
#include "textmanager.h"

#include "hashtables.h"
#include "console.h"
#include "stringlib.h"


#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"
int text_ok = 0;
int textcount = 0;
int textArrayFirstOpen = 0;
int textArrayLastTaken = -1;
int textArraySize = 0;
int fontcount = 0;
int fontArrayFirstOpen = 0;
int fontArrayLastTaken = -1;
int fontArraySize = 0;


//hashbucket_t texthashtable[MAXHASHBUCKETS];
hashbucket_t fonthashtable[MAXHASHBUCKETS];

//text_t *textlist;
font_t *fontlist;

int text_init(void){
//	memset(texthashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
//	if(textlist) free(textlist);
//	textlist = malloc(sizeof(text_t));
//	memset(textlist, 0 , sizeof(text_t));
	memset(fonthashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(fontlist) free(fontlist);
	fontlist = malloc(sizeof(font_t));
	memset(fontlist, 0 , sizeof(font_t));
//	textlist = malloc(textnumber * sizeof(text_t));
//	if(!textlist) memset(textlist, 0, textnumber * sizeof(text_t));
//	defaulttext = addtextToList(createtext("default", 0));
	//todo error checking

//	if(TTF_Init() == -1){
//		return FALSE;
//	}
	text_ok = TRUE;
	return TRUE;
}
/*
text_t * findTextByNameRPOINT(const char * name){
//	console_printf("text id:%i\n", findByNameRINT(name, texthashtable));
//	if(!textlist) return FALSE;
	return returnTextById(findByNameRINT(name, texthashtable));
}
int findTextByNameRINT(const char * name){
//	if(!textlist) return FALSE;
	return findByNameRINT(name, texthashtable);
}
textlistpoint_t findTextsByNameRPOINT(const char * name){
	textlistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &texthashtable[hash];
	if(!hb->name) return ret;
	for(; hb; hb = hb->next){
		if(string_testEqual(hb->name, name)){
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(text_t *));
			ret.list[ret.count-1] = returnTextById(hb->id);
		}
	}
	return ret;
}
textlistint_t findTextsByNameRINT(const char * name){
	textlistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &texthashtable[hash];
	if(!hb->name) return ret;
	for(; hb; hb = hb->next){
		if(string_testEqual(hb->name, name)){
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
	}
	return ret;
}

*/
font_t * findFontByNameRPOINT(const char * name){
	if(!fontlist) return FALSE;
	return returnFontById(findByNameRINT(name, fonthashtable));
}
int findFontByNameRINT(const char * name){
	if(!fontlist) return FALSE;
	return findByNameRINT(name, fonthashtable);
}
fontlistpoint_t findFontsByNameRPOINT(const char * name){
	fontlistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &fonthashtable[hash];
	if(!hb->name) return ret;
	for(; hb; hb = hb->next){
		if(string_testEqual(hb->name, name)){
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(font_t *));
			ret.list[ret.count-1] = returnFontById(hb->id);
		}
	}
	return ret;
}
fontlistint_t findFontsByNameRINT(const char * name){
	fontlistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &fonthashtable[hash];
	if(!hb->name) return ret;
	for(; hb; hb = hb->next){
		if(string_testEqual(hb->name, name)){
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
	}
	return ret;
}
/*
int deleteText(const int id){
	int textindex = (id & 0xFFFF);
	text_t * tex = &textlist[textindex];
	if(tex->myid != id) return FALSE;
	if(!tex->name) return FALSE;
	glDeleteTextures(1, &tex->textureid);
	deleteFromHashTable(tex->name, id, texthashtable);
	free(tex->name);
	memset(tex,0, sizeof(text_t));
	if(textindex < textArrayFirstOpen) textArrayFirstOpen = textindex;
	for(; textArrayLastTaken > 0 && !textlist[textArrayLastTaken].type; textArrayLastTaken--);
	return TRUE;
}

int deleteUnmarkedText(void){
	unsigned int count = 0, i;
	for(i = 0; i < textArrayLastTaken; i++){
		text_t * tex = &textlist[i];
		if(tex->type && tex->type < 3){
			count++;
			deleteText(tex->myid);
		}
	}
	return count;
}
*/
int deleteFont(const int id){
	int fontindex = (id & 0xFFFF);
	font_t * tex = &fontlist[fontindex];
	if(tex->myid != id) return FALSE;
	if(!tex->filename) return FALSE;
	if(tex->ttfdata) free(tex->ttfdata);
	deleteFromHashTable(tex->filename, id, fonthashtable);
	free(tex->filename);
	if(tex->cdata) free(tex->cdata);
	memset(tex, 0, sizeof(font_t));
	if(fontindex < fontArrayFirstOpen) fontArrayFirstOpen = fontindex;
	for(; fontArrayLastTaken > 0 && !fontlist[fontArrayLastTaken].type; fontArrayLastTaken--);
	//todo maybe look up all texts that use this font and delete em?
	return TRUE;
}
/*
text_t * returnTextById(const int id){
	int textindex = (id & 0xFFFF);
	text_t * tex = &textlist[textindex];
	if(!tex->type) return FALSE;
	if(tex->myid == id) return tex;
	return FALSE;
}
*/
font_t * returnFontById(const int id){
	int fontindex = (id & 0xFFFF);
	font_t * tex = &fontlist[fontindex];
	if(!tex->type) return FALSE;
	if(tex->myid == id) return tex;
	return FALSE;
}
/*
text_t createAndRenderTextFindFont(const char * name, const char * fontname, const unsigned int x, const unsigned int y, const float fontsize){
	int f = createAndAddFontRINT(fontname);
	return createAndRenderText(name, f, x, y, fontsize);
}

//todo font size, style, color, and text either blended or shaded (for alpha blended/tested or no transparency)
//todo font is only a fontid, font lookup by name and size will be done with a helper function for speed
text_t createAndRenderText(const char * name, const int font, const unsigned int x, const unsigned int y, const float fontsize){
	text_t tex;
	tex.type = 1;
	tex.numchars = strlen(name);

	tex.name = malloc(tex.numchars+1);
	strcpy(tex.name, name);

	font_t *f = returnFontById(font);

	if(!f)return tex;
	if(!f->ttfdata || f->type < 2) return tex;



	void * data = malloc(x * y);
	tex.width = x;
	tex.height = y;
	tex.fontsize = fontsize;
	//find mipmap level
	unsigned char level;
	for(level = 0; level < 255; level++){
		if(1<<level > tex.width && 1<<level > tex.height) break;
	}
	stbtt_BakeFontBitmap(f->ttfdata, 0, fontsize, data, x, y, 32, tex.numchars, tex.name); // no guarantee this fits

	glGenTextures(1, &tex.textureid);
	glBindTexture(GL_TEXTURE_2D, tex.textureid);
	glTexImage2D(GL_TEXTURE_2D, 0 , GL_ALPHA, tex.width, tex.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);//Anisotropic Filtering Attempt
	glGenerateMipmap(GL_TEXTURE_2D);
	free(data);
	tex.type = 2;
	return tex;
}
*/
//todo font size
font_t createAndLoadFont(const char * filename){
	font_t font;
	font.type = 1;
//	font.size = size;
	font.filename = malloc(strlen(filename)+1);
	strcpy(font.filename, filename);

	FILE * pFile; //todo filesys
	pFile = fopen (font.filename , "rb" );
	if(!pFile){
		console_printf("Error loading font %s, File does not exist!\n", font.filename);
		return font;
	}
	fseek(pFile, 0, SEEK_END);
	font.datasize = ftell(pFile);
	rewind(pFile);
	font.ttfdata = malloc(font.datasize);
	fread(font.ttfdata, 1, font.datasize, pFile);
	fclose(pFile);
//	font.font = TTF_OpenFont(font.filename, size); //todo size
	if(!font.ttfdata) return font;
	font.height = 512;
	font.width = 512;
	float fontsize = 32.0;



	void * data = malloc(font.height * font.width);
	//find mipmap level
	unsigned char level;
	for(level = 0; level < 255; level++){
		if(1<<level > font.width && 1<<level > font.height) break;
	}
	font.cdata = malloc(96 * sizeof(stbtt_bakedchar));

	stbtt_BakeFontBitmap(font.ttfdata, 0, fontsize, data, font.width, font.height, 32, 96, font.cdata); // no guarantee this fits
	glGenTextures(1, &font.textureid);
	states_bindTexture(GL_TEXTURE_2D, font.textureid);
	glTexImage2D(GL_TEXTURE_2D, 0 , GL_ALPHA, font.width, font.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);//Anisotropic Filtering Attempt
	glGenerateMipmap(GL_TEXTURE_2D);
	free(data);

	font.type = 2;
	console_printf("loaded font %s with size %i\n", filename, font.datasize);
	return font;
}
/*
int deleteAllText(void){
	int i;
	for(i = 0; i < textnumber; i++){
		deleteText(textlist[i].myid); // if texgroup is unused/deleted, the num will be 0 anyway
	}
	free(textlist);
	textlist = 0;
	textnumber = 0;
	return i; //todo useful returns
}
*/
int deleteAllFont(void){
	int i;
	for(i = 0; i < fontnumber; i++){
		deleteFont(fontlist[i].myid); // if texgroup is unused/deleted, the num will be 0 anyway
	}
	free(fontlist);
	fontlist = 0;
	fontnumber = 0;
	return i; //todo useful returns
}
/*
int addTextRINT(text_t tex){
	textcount++;
	for(; textArrayFirstOpen < textArraySize && textlist[textArrayFirstOpen].type; textArrayFirstOpen++);
	if(textArrayFirstOpen == textArraySize){	//resize
		textArraySize++;
		textlist = realloc(textlist, textArraySize * sizeof(text_t));
	}
	textlist[textArrayFirstOpen] = tex;
	int returnid = (textcount << 16) | textArrayFirstOpen;
	textlist[textArrayFirstOpen].myid = returnid;

	addToHashTable(textlist[textArrayFirstOpen].name, returnid, texthashtable);
	if(textArrayLastTaken < textArrayFirstOpen) textArrayLastTaken = textArrayFirstOpen; //todo redo
	return returnid;
}
text_t * addTextRPOINT(text_t tex){
	textcount++;
	for(; textArrayFirstOpen < textArraySize && textlist[textArrayFirstOpen].type; textArrayFirstOpen++);
	if(textArrayFirstOpen == textArraySize){	//resize
		textArraySize++;
		textlist = realloc(textlist, textArraySize * sizeof(text_t));
	}
	textlist[textArrayFirstOpen] = tex;
	int returnid = (textcount << 16) | textArrayFirstOpen;
	textlist[textArrayFirstOpen].myid = returnid;

	addToHashTable(textlist[textArrayFirstOpen].name, returnid, texthashtable);
	//todo maybe have text have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(textArrayLastTaken < textArrayFirstOpen) textArrayLastTaken = textArrayFirstOpen;
//	printf("textarraysize = %i\n", textArraySize);
//	printf("textcount = %i\n", textcount);

	return &textlist[textArrayFirstOpen];

}
*/
int addFontRINT(font_t tex){
	fontcount++;
	for(; fontArrayFirstOpen < fontArraySize && fontlist[fontArrayFirstOpen].type; fontArrayFirstOpen++);
	if(fontArrayFirstOpen == fontArraySize){	//resize
		fontArraySize++;
		fontlist = realloc(fontlist, fontArraySize * sizeof(font_t));
	}
	fontlist[fontArrayFirstOpen] = tex;
	int returnid = (fontcount << 16) | fontArrayFirstOpen;
	fontlist[fontArrayFirstOpen].myid = returnid;

	addToHashTable(fontlist[fontArrayFirstOpen].filename, returnid, fonthashtable);
	if(fontArrayLastTaken < fontArrayFirstOpen) fontArrayLastTaken = fontArrayFirstOpen; //todo redo
	return returnid;
}
font_t * addFontRPOINT(font_t tex){
	fontcount++;
	for(; fontArrayFirstOpen < fontArraySize && fontlist[fontArrayFirstOpen].type; fontArrayFirstOpen++);
	if(fontArrayFirstOpen == fontArraySize){	//resize
		fontArraySize++;
		fontlist = realloc(fontlist, fontArraySize * sizeof(font_t));
	}
	fontlist[fontArrayFirstOpen] = tex;
	int returnid = (fontcount << 16) | fontArrayFirstOpen;
	fontlist[fontArrayFirstOpen].myid = returnid;

	addToHashTable(fontlist[fontArrayFirstOpen].filename, returnid, fonthashtable);
	//todo maybe have font have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(fontArrayLastTaken < fontArrayFirstOpen) fontArrayLastTaken = fontArrayFirstOpen;
//	printf("fontarraysize = %i\n", fontArraySize);
//	printf("fontcount = %i\n", fontcount);

	return &fontlist[fontArrayFirstOpen];

}
/*
int createAndAddTextFindFontRINT(const char * name, const char * fontname, const unsigned int x, const unsigned int y, const float fontsize){
	int m = findTextByNameRINT(name); //todo list and make sure that is is same font/size
//	console_printf("text id: %i\n", m);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRINT(createAndRenderTextFindFont(name, fontname, x, y, fontsize));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}

text_t * createAndAddTextFindFontRPOINT(const char * name, const char * fontname, const unsigned int x, const unsigned int y, const float fontsize){
	text_t * m = findTextByNameRPOINT(name);
//	if(m) console_printf("text id: %i\n", m->myid);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRPOINT(createAndRenderTextFindFont(name, fontname, x, y, fontsize));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}

int createAndAddTextRINT(const char * name,const int font, const unsigned int x, const unsigned int y, const float fontsize){
	int m = findTextByNameRINT(name); //todo list and make sure that is is same font/size
//	console_printf("text id: %i\n", m);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRINT(createAndRenderText(name, font, x, y, fontsize));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}

text_t * createAndAddTextRPOINT(const char * name, const int font, const unsigned int x, const unsigned int y, const float fontsize){
	text_t * m = findTextByNameRPOINT(name);
//	if(m) console_printf("text id: %i\n", m->myid);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRPOINT(createAndRenderText(name, font, x, y, fontsize));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}
*/
int createAndAddFontRINT(const char * name){
	int m = findFontByNameRINT(name);
//	console_printf("font id: %i\n", m);
	if(m) return m;
	return addFontRINT(createAndLoadFont(name));
//	return &fontlist[addFontToList(createAndLoadFont(name))];
}
font_t * createAndAddFontRPOINT(const char * name){
	font_t * m = findFontByNameRPOINT(name);
//	if(m) console_printf("font id: %i\n", m->myid);
	if(m) return m;
	return addFontRPOINT(createAndLoadFont(name));
//	return &fontlist[addFontToList(createAndLoadFont(name))];
}
