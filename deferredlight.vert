//todo put something in shadermanager to handle this
//something like a define instancesize 16, then have the shadermanager put in a define that does the devide?

uniform mat4 unimat40; //mvp
uniform mat4 unimat41; // mv

//uniform vec2 uniscreentodepth;


in vec3 posattrib;
#ifdef SPOT
#define N 256
struct lightdata {
	mat4 mvp;
	mat4 mv;
	vec3 pos;
	float size;
};
#else
#define N 4096
struct lightdata {
	vec3 pos;
	float size;
};
#endif

layout (std140) uniform uniblock0 {
	lightdata ldata[N]; //todo
} uniblock0_t;

out vec3 mvpos; // vertex position in viewspace

#ifdef DIRECTIONAL
	flat out vec3 lightdirection;
#else
	#ifdef SPOT
	#else
		flat out vec3 lpos; // light position in viewspace
		flat out float lsize; // size of light
	#endif
#endif



void main(){
	lightdata l = uniblock0_t.ldata[gl_InstanceID];
	#ifdef DIRECTIONAL
		lightdirection = (unimat41 * vec4(l.pos, 0.0)).xyz;
	#else
		#ifdef SPOT
			gl_Position = l.mvp * vec4(posattrib, 1.0);
//			vec3 translated = (posattrib * l.size) + l.pos;
//			gl_Position = unimat40 * vec4(translated, 1.0);
		#else
			vec3 translated = (posattrib * l.size) + l.pos;
			lsize = l.size;
			lpos = (unimat41 * vec4(l.pos, 1.0)).xyz; // viewspace of light
			mvpos = (unimat41 * vec4(translated, 1.0)).xyz; //viewspace of the mvpos
			gl_Position = unimat40 * vec4(translated, 1.0);
		#endif
	#endif
}
