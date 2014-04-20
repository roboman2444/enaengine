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
	textlist = 0;
	memset(fonthashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(fontlist) free(fontlist);
	fontlist = 0;
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
	return returnTextById(findByNameRINT(name, texthashtable));
}
int findTextByNameRINT(char * name){
	return findByNameRINT(name, texthashtable);
}
font_t * findFontByNameRPOINT(char * name){
	return returnFontById(findByNameRINT(name, fonthashtable));
}
int findFontByNameRINT(char * name){
	return findByNameRINT(name, fonthashtable);
}
int deleteText(int id){
	int textindex = (id & 0xFFFF);
	text_t * tex = &textlist[textindex];
	if(tex->myid != id) return FALSE;
	if(!tex->name) return FALSE;
	int i = 0;
/*
	if(tex->texture){
		for(i = 0; i < tex->num; i++){
			deleteTexture(tex->textures[i]);
		}
		free(tex->textures);
	}
*/
	free(tex->name);
	memset(tex,0, sizeof(text_t));
	if(textindex < textArrayFirstOpen) textArrayFirstOpen = textindex;
	for(; textArrayLastTaken > 0 && !textlist[textArrayLastTaken].type; textArrayLastTaken--);
	return i;
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
	if(!s) return tex;
	glGenTextures(1, &tex.textureid);
	glBindTexture(GL_TEXTURE_2D, tex.textureid);
	glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
	//todo mipmaps and sampling
	tex.width = s->w;
	tex.height = s->h;
	SDL_FreeSurface(s);
	tex.type = 2;
	return tex;
}
font_t createAndLoadFont(char * filename /*,int size*/){
	font_t font;
	font.type = 1;
	font.filename = malloc(strlen(filename)+1);
	strcpy(font.filename, filename);
	font.font = TTF_OpenFont(font.filename, 14 ); //todo size
	if(!font.font) return font;
	font.type = 2;
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
int createAndAddTextRINT(char * name, char * fontname){
	int m = findTextByNameRINT(name); //todo list and make sure that is is same font/size
	if(m) return m;
	return addTextRINT(createAndRenderText(name, fontname));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}
text_t * createAndAddTextRPOINT(char * name, char * fontname){
	text_t * m = findTextByNameRPOINT(name);
	if(m) return m;
	return addTextRPOINT(createAndRenderText(name, fontname));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}

int createAndAddFontRINT(char * name){
	int m = findFontByNameRINT(name);
	if(m) return m;
	return addFontRINT(createAndLoadFont(name));
//	return &fontlist[addFontToList(createAndLoadFont(name))];
}
font_t * createAndAddFontRPOINT(char * name){
	font_t * m = findFontByNameRPOINT(name);
	if(m) return m;
	return addFontRPOINT(createAndLoadFont(name));
//	return &fontlist[addFontToList(createAndLoadFont(name))];
}
