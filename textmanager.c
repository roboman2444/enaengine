#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys


#include "globaldefs.h"
#include "textmanager.h"

#include "SDL_ttf.h"
#include "SDL.h"
#include "hashtables.h"
#include "console.h"
int text_ok = 0;
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

int text_init(void){
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
	text_ok = TRUE;
	return TRUE;
}

text_t * findTextByNameRPOINT(char * name){
//	console_printf("text id:%i\n", findByNameRINT(name, texthashtable));
//	if(!textlist) return FALSE;
	return returnTextById(findByNameRINT(name, texthashtable));
}
int findTextByNameRINT(char * name){
//	if(!textlist) return FALSE;
	return findByNameRINT(name, texthashtable);
}
textlistpoint_t findTextsByNameRPOINT(char * name){
	textlistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &texthashtable[hash];
	if(!hb->name) return ret;
	for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(text_t *));
			ret.list[ret.count-1] = returnTextById(hb->id);
		}
	}
	return ret;
}
textlistint_t findTextsByNameRINT(char * name){
	textlistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &texthashtable[hash];
	if(!hb->name) return ret;
	for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
	}
	return ret;
}


font_t * findFontByNameRPOINT(char * name){
	if(!fontlist) return FALSE;
	return returnFontById(findByNameRINT(name, fonthashtable));
}
int findFontByNameRINT(char * name){
	if(!fontlist) return FALSE;
	return findByNameRINT(name, fonthashtable);
}
fontlistpoint_t findFontsByNameRPOINT(char * name){
	fontlistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &texthashtable[hash];
	if(!hb->name) return ret;
	for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(font_t *));
			ret.list[ret.count-1] = returnFontById(hb->id);
		}
	}
	return ret;
}
fontlistint_t findFontsByNameRINT(char * name){
	fontlistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &texthashtable[hash];
	if(!hb->name) return ret;
	for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
	}
	return ret;
}

font_t * findFontByNameSizeRPOINT(char * name, unsigned short size){
//	if(!fontlist) return FALSE;
	int hash = getHash(name);
	hashbucket_t *hb = &fonthashtable[hash];
	if(!hb->name) return FALSE;
	for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
			font_t * ret = returnFontById(hb->id);
			if(ret->size == size) return ret;
		}
	}
	return FALSE;
}
int findFontByNameSizeRINT(char * name, unsigned short size){
//	if(!fontlist) return FALSE;
	int hash = getHash(name);
	hashbucket_t *hb = &fonthashtable[hash];
	if(!hb->name) return FALSE;
	for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
			font_t * ret = returnFontById(hb->id);
			if(ret->size == size) return ret->myid;
		}
	}
	return FALSE;
}


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
int deleteFont(const int id){
	int fontindex = (id & 0xFFFF);
	font_t * tex = &fontlist[fontindex];
	if(tex->myid != id) return FALSE;
	if(!tex->filename) return FALSE;
	if(tex->font) TTF_CloseFont((TTF_Font *)tex->font);
	deleteFromHashTable(tex->filename, id, fonthashtable);
	free(tex->filename);
	memset(tex, 0, sizeof(text_t));
	if(fontindex < fontArrayFirstOpen) fontArrayFirstOpen = fontindex;
	for(; fontArrayLastTaken > 0 && !fontlist[fontArrayLastTaken].type; fontArrayLastTaken--);
	return TRUE;
}
text_t * returnTextById(const int id){
	int textindex = (id & 0xFFFF);
	text_t * tex = &textlist[textindex];
	if(!tex->type) return FALSE;
	if(tex->myid == id) return tex;
	return FALSE;
}
font_t * returnFontById(const int id){
	int fontindex = (id & 0xFFFF);
	font_t * tex = &fontlist[fontindex];
	if(!tex->type) return FALSE;
	if(tex->myid == id) return tex;
	return FALSE;
}
text_t createAndRenderTextFindFont(char * name, char * fontname, unsigned short size, char style, char fgcolor[3]){
	int f = createAndAddFontRINT(fontname, size);
	return createAndRenderText(name, f, style, fgcolor);
}
//todo font size, style, color, and text either blended or shaded (for alpha blended/tested or no transparency)
//todo font is only a fontid, font lookup by name and size will be done with a helper function for speed
text_t createAndRenderText(char * name, int font, char style, char fgcolor[3]){
	//todo bgcolor
	//todo bold italic etc
	text_t tex;
	tex.type = 1;
	tex.numchars = strlen(name);

	tex.name = malloc(tex.numchars+1);
	strcpy(tex.name, name);

	font_t *f = returnFontById(font);

	if(!f)return tex;
	if(!f->font || f->type < 2) return tex;
	SDL_Surface * s;
	SDL_Color textColor;// = fgcolor;
	textColor.r = fgcolor[0];
	textColor.g = fgcolor[1];
	textColor.b = fgcolor[2];
//	SDL_Color textColor = {255, 128, 128};
	SDL_Color backColor = {0, 0, 0};
	//blended does argb
	if(style & TEXT_FORMAT_ALPHA) s = TTF_RenderText_Blended((TTF_Font*)f->font, name, textColor);
	else s = TTF_RenderText_Shaded((TTF_Font*)f->font, name, textColor, backColor);
//	s = TTF_RenderText_Solid((TTF_Font*)f->font, name, textColor);
	if(!s) return tex;
	tex.width = s->w;
	tex.height = s->h;

//flips the image vertically
	int  y, posy, multwidth;
	char * newdata;
	if(style & TEXT_FORMAT_ALPHA){
		newdata = malloc(tex.width * tex.height * 4);
		multwidth = tex.width *4;
		for(posy = tex.height-1, y = 0; y < tex.height; posy--, y++){
			memcpy(newdata + y*multwidth, s->pixels + posy*multwidth, multwidth);
		}
	} else {
	/*
		newdata = malloc(tex.width * tex.height * 3);
		multwidth = tex.width * 3;
		for(posy = tex.height-1, y = 0; y < tex.height; posy--, y++){
			int x;
			for(x = 0; x < tex.width; x++)
			((char *)newdata)[(y*multwidth)+(x*3)+0] = ((char *)s->pixels)[(posy*tex.width)+x] * fgcolor[0];
			((char *)newdata)[(y*multwidth)+(x*3)+1] = ((char *)s->pixels)[(posy*tex.width)+x] * fgcolor[1];
			((char *)newdata)[(y*multwidth)+(x*3)+2] = ((char *)s->pixels)[(posy*tex.width)+x] * fgcolor[2];
//			memcpy(newdata + y*multwidth, s->pixels + posy*multwidth, multwidth);
		}
	*/
	//todo fix
		newdata = malloc(tex.width * tex.height);
		for(posy = tex.height-1, y = 0; y < tex.height; posy--, y++){
			memcpy(newdata + y*tex.width, s->pixels + posy*tex.width, tex.width);
		}

	}


	//find mipmap level
	unsigned char level;
	for(level = 0; level < 255; level++){
		if(1<<level > tex.width && 1<<level > tex.height) break;
	}

	glGenTextures(1, &tex.textureid);
	glBindTexture(GL_TEXTURE_2D, tex.textureid);
//	glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
	if(style & TEXT_FORMAT_ALPHA) glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newdata);
//	else glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGB, tex.width, tex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, newdata);
	else glTexImage2D(GL_TEXTURE_2D, 0 , GL_RED, tex.width, tex.height, 0, GL_RED, GL_UNSIGNED_BYTE, newdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);//Anisotropic Filtering Attempt
	glGenerateMipmap(GL_TEXTURE_2D);
	free(newdata);
	SDL_FreeSurface(s);

	tex.type = 2;
	tex.style = style;
	tex.color[0] = fgcolor[0];
	tex.color[1] = fgcolor[1];
	tex.color[2] = fgcolor[2];
//	console_printf("rendered text \"%s\" with font %s\n", name, f->filename );
//	printf("rendered text \"%s\" with font %s\n", name, f->filename );
	return tex;
}
//todo font size
font_t createAndLoadFont(char * filename ,unsigned short size){
	font_t font;
	font.type = 1;
	font.size = size;
	font.filename = malloc(strlen(filename)+1);
	strcpy(font.filename, filename);
	font.font = TTF_OpenFont(font.filename, size); //todo size
	if(!font.font) return font;
	font.type = 2;
	console_printf("loaded font %s with size %i\n", filename, size);
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
int createAndAddTextFindFontRINT(char * name, char * fontname, unsigned short size, char style, char fgcolor[3]){
	int m = findTextByNameRINT(name); //todo list and make sure that is is same font/size
//	console_printf("text id: %i\n", m);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRINT(createAndRenderTextFindFont(name, fontname, size, style, fgcolor));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}
text_t * createAndAddTextFindFontRPOINT(char * name, char * fontname, unsigned short size, char style, char fgcolor[3]){
	text_t * m = findTextByNameRPOINT(name);
//	if(m) console_printf("text id: %i\n", m->myid);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRPOINT(createAndRenderTextFindFont(name, fontname, size, style, fgcolor));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}

int createAndAddTextRINT(char * name, int font, char style, char fgcolor[3]){
	int m = findTextByNameRINT(name); //todo list and make sure that is is same font/size
//	console_printf("text id: %i\n", m);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRINT(createAndRenderText(name, font, style, fgcolor));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}
text_t * createAndAddTextRPOINT(char * name, int font, char style, char fgcolor[3]){
	text_t * m = findTextByNameRPOINT(name);
//	if(m) console_printf("text id: %i\n", m->myid);
	if(m) return m; //todo instead of return single, return LIST and check that its the same size and font
	return addTextRPOINT(createAndRenderText(name, font, style, fgcolor));
//	return &textlist[addtextToList(createAndLoadtext(name))];
}

int createAndAddFontRINT(char * name, unsigned short size){
	int m = findFontByNameSizeRINT(name, size);
//	console_printf("font id: %i\n", m);
	if(m) return m;
	return addFontRINT(createAndLoadFont(name, size));
//	return &fontlist[addFontToList(createAndLoadFont(name))];
}
font_t * createAndAddFontRPOINT(char * name, unsigned short size){
	font_t * m = findFontByNameSizeRPOINT(name, size);
//	if(m) console_printf("font id: %i\n", m->myid);
	if(m) return m;
	return addFontRPOINT(createAndLoadFont(name, size));
//	return &fontlist[addFontToList(createAndLoadFont(name))];
}
