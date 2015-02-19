#include "globaldefs.h"
#include <string.h> //well duh

#include "stringlib.h"


/*
vec3_t string_toVec3(const char *s){
	unsigned int i;
	vec3_t v;
	for(i = 0; i < 3; i++){
		while(*s && ISWHITESPACE(*s)) s++;
		if(!*s) break;
		v[i] = atof(s);
		while(!ISWHITESPACE(*s)) s++;
	}
	return v;
}
*/
//returns TRUE if both strings are equal, false otherwise
//suitable replacement for !strcmp() if you only want to check if they are the same
int string_testEqual(const char * one, const char * two){
	if(!one || !two) return FALSE;
	char * m1 = (char *)one;
	char * m2 = (char *)two;
	//i dont test both in the while because checking weather they are equal does it for me
	while(*m1) if(*m1++ != *m2++) return FALSE;
	//now check if m1 stopped early and m2 still goes
	if(*m2) return FALSE;
	return TRUE;
}

//returns TRUE if both strings are equal, false otherwise. tests only first n characters
//suitable replacement for !strcmp() if you only want to check if they are the same
//unsigned int because strings might be >2gb. Call the string length police i dont care.
int string_testEqualN(const char * one, const char * two, const unsigned int n){
	if(!one || !two) return FALSE;
	char * m1 = (char *)one;
	char * m2 = (char *)two;
	unsigned int i;
	//i dont test both in the while because checking weather they are equal does it for me
	for(i = 0; i < n && *m1; i++) if(*m1++ != *m2++) return FALSE;
	return TRUE;
}

unsigned int string_toVec(const char *s, vec_t *v, const unsigned int maxdem){
	unsigned int i;
	for(i = 0; i < maxdem; i++){
		while(*s && ISWHITESPACE(*s)) s++;
		if(!*s) break;
		v[i] = atof(s);
		while(!ISWHITESPACE(*s)) s++;
	}
	//set rest of them to 0.0
	unsigned int j;
	for(j = i; j < maxdem; j++) v[j] = 0.0;
	return j;
}
