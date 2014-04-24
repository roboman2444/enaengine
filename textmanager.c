#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys


#include "globaldefs.h"
#include "textmanager.h"

#include "SDL_ttf.h"
#include "SDL.h"
#include "hashtables.h"
#include "console.h"
int textOK = 0;
int textcount = 0;
int textArrayFirstOpen = 0;
int textArrayLastTaken = -1;
int textArraySize = 0;
int fontcount = 0;
int fontArrayFirstOpen = 0;
int fontArrayLastTaken = -1;
int fontArraySize = 0;


hashbucket_t texthashtable[MAXHASHBUCKETS];
hashbucket_t fonthashtable[MAXHASHBUCKETS];

text_t *textlist;
font_t *fontlist;

int initTextSystem(void){
	memset(texthashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(textlist) free(textlist);
	textlist = malloc(sizeof(text_t));
	memset(textlist, 0 , sizeof(text_t));
	memset(fonthashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(fontlist) free(fontlist);
	fontlist = malloc(sizeof(font_t));
	memset(fontlist, 0 , sizeof(font_t));
//	textlist = malloc(textnumber * sizeof(text_t));
//	if(!textlist) memset(textlist, 0, textnumber * sizeof(text_t));
//	defaulttext = addtextToList(createtext("default", 0));
	//todo error checking

	if(TTF_Init() == -1){
		return FALSE;
	}
	textOK = TRUE;
	return TRUE;
}

text_t * findTextByNameRPOINT(char * name){
//	consolePrintf("text id:%i\n", findByNameRINT(name, texthashtable));
//	if(!textlist) return FALSE;
	return returnTextById(findByNameRINT(name, texthashtable));
}
int findTextByNameRINT(char * name){
//	if(!textlist) return FALSE;
	return findByNameRINT(name, texthashtable);
}
font_t * findFontByNameRPOINT(char * name){
	if(!fontlist) return FALSE;
	return returnFontById(findByNameRINT(name, fonthashtable));
}
int findFontByNameRINT(char * name){
	if(!fontlist) return FALSE;
	return findByNameRINT(name, fonthashtable);
}
int deleteText(int id){
	int textindex = (id & 0xFFFF);
	text_t * tex = &textlist[textindex];
	if(tex->myid != id) return FALSE;
	if(!tex->name) return FALSE;
	glDeleteTextures(1, &tex->textureid);
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
int deleteFont(int id){
	int fontindex = (id & 0xFFFF);
	font_t * tex = &fontlist[fontindex];
	if(tex->myid != id) return FALSE;
	if(!tex->filename) return FALSE;
	if(tex->font) TTF_CloseFont((TTF_Font *)tex->font);
	free(tex->filename);
	memset(tex, 0, sizeof(text_t));
	if(fontindex < fontArrayFirstOpen) fontArrayFirstOpen = fontindex;
	for(; fontArrayLastTaken > 0 && !fontlist[fontArrayLastTaken].type; fontArrayLastTaken--);
	return TRUE;
}
text_t * returnTextById(int id){
	int textindex = (id & 0xFFFF);
	text_t * tex = &textlist[textindex];
	if(!tex->type) return FALSE;
	if(tex->myid == id) return tex;
	return FALSE;
}
font_t * returnFontById(int id){
	int fontindex = (id & 0xFFFF);
	font_t * tex = &fontlist[fontindex];
	if(!tex->type) return FALSE;
	if(tex->myid == id) return tex;
	return FALSE;
}
//todo font size, style, color, and text either blended or shaded (for alpha blended/tested or no transparency)
text_t createAndRenderText(char * name, char * fontname){
	text_t tex;
	tex.type = 1;
	tex.numchars = strlen(name);

	tex.name = malloc(tex.numchars+1);
	strcpy(tex.name, name);

	font_t * f = createAndAddFontRPOINT(fontname);
	if(!f)return tex;
	if(!f->font || f->type < 2) return tex;
	SDL_Surface * s;
	SDL_Color textColor = {255, 255, 255};
	//blended does argb
	s = TTF_RenderText_Blended((TTF_Font*)f->font, name, textColor);
//	s = TTF_RenderText_Solid((TTF_Font*)f->font, name, textColor);
	if(!s) return tex;
	tex.width = s->w;
	tex.height = s->h;

	char * newdata = malloc(tex.width * tex.height * 4);
//flips the image vertically
	int  y, posy, multwidth = tex.width *4;
	for(posy = tex.height-1, y = 0; y < tex.height; posy--, y++){
		memcpy(newdata + y*multwidth, s->pixels + posy*multwidth, multwidth);
	}

	unsigned char level;
	for(level = 0; level < 255; level++){
		if(1<<level > tex.width && 1<<level > tex.height) break;
	}

	glGenTextures(1, &tex.textureid);
	glBindTexture(GL_TEXTURE_2D, tex.textureid);
//	glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
	glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(newdata);
	SDL_FreeSurface(s);
	//todo mipmaps and sampling
	tex.type = 2;
	consolePrintf("rendered text \"%s\" with font %s\n", name, fontname );
	return tex;
}
//todo font size
font_t createAndLoadFont(char * filename /*,int size*/){
	font_t font;
	font.type = 1;
	font.filename = malloc(strlen(filename)+1);
	strcpy(font.filename, filename);
	font.font = TTF_OpenFont(font.filename, 256 ); //todo size
	if(!font.font) return font;
	font.type = 2;
	consolePrintf("loaded font %s\n", filename);
	return font;
}


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
int deleteAllFont(void){
	int i;
	for(i = 0; i < fontnumber; i++){
		deleteFont(textlist[i].myid); // if texgroup is unused/deleted, the num will be 0 anyway
	}
	free(fontlist);
	fontlist = 0;
	fontnumber = 0;
	return i; //todo useful returns
}
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
int createAndAddTextRINT(char * name, char * fontname/*, int size*/){
	int m = findTextByNameRINT(name); //todo list and make sure that is is same font/size
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRINT(createAndRenderText(name, fontname /*, size*/));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}
text_t * createAndAddTextRPOINT(char * name, char * fontname /*, int size*/){
	text_t * m = findTextByNameRPOINT(name);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRPOINT(createAndRenderText(name, fontname /*, size*/));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}

int createAndAddFontRINT(char * name /*, int size*/){
	int m = findFontByNameRINT(name);
	if(m) return m;
	return addFontRINT(createAndLoadFont(name/*, size*/));
//	return &fontlist[addFontToList(createAndLoadFont(name))];
}
font_t * createAndAddFontRPOINT(char * name /*, int size*/){
	font_t * m = findFontByNameRPOINT(name);
	if(m) return m;
	return addFontRPOINT(createAndLoadFont(name /*, size*/));
//	return &fontlist[addFontToList(createAndLoadFont(name))];
}
