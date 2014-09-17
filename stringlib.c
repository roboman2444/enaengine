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
