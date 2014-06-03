#include "globaldefs.h"
#include "math.h"
#include "mathlib.h"
void getBBoxpFromBBox(vec_t * bbox, vec_t *bboxp){
	int i;
	for(i = 0; i < 8; i++){
		bboxp[(i*3)+0] = bbox[(i&1)+0];
		bboxp[(i*3)+1] = bbox[((i&2)>>1)+2];
		bboxp[(i*3)+2] = bbox[((i&4)>>2)+4];
	}
}
vec_t vec3distvec(vec3_t b, vec3_t c){
	vec3_t math;
	vec3subvec(math, b, c);
	return vec3length(math);
}
vec_t vec3distfastvec(vec3_t b, vec3_t c){
	vec3_t math;
	vec3subvec(math, b, c);
	return vec3dot(math, math);
}
vec_t vec4distvec(vec4_t b, vec4_t c){
	vec4_t math;
	vec4subvec(math, b, c);
	return vec4length(math);
}
vec_t vec4distfastvec(vec4_t b, vec4_t c){
	vec4_t math;
	vec4subvec(math, b, c);
	return vec4dot(math, math);
}
/*
vec_t * vec3norm(vec3_t b){
	vec_t length = vec3length(b);
	vec3_t out;
	vec3div(out, b, length);
	return out;
}
vec_t * vec4norm(vec4_t b){
	vec_t length = vec4length(b);
	vec4_t out;
	vec4div(out, b, length);
	return out;
}
*/
void vec3norm2(vec3_t a, vec3_t b){
	vec_t length = vec3length(b);
	vec3div(a, b, length);
}
void vec4norm2(vec4_t a, vec4_t b){
	vec_t length = vec4length(b);
	vec4div(a, b, length);
}
