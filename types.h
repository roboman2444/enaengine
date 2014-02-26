#ifndef TYPESHEADER
#define TYPESHEADER
//pretty much copied from quake
#ifdef VEC_64
	typedef double vec_t;
#else
	typedef float vec_t;
#endif
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];
typedef vec_t vec6_t[6];
typedef vec_t vec7_t[7];
typedef vec_t vec8_t[8];

typedef struct plane_s {
	vec3_t norm;
	vec_t d;
} plane_t;
#endif
