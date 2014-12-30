//todo put something in shadermanager to handle this
//something like a define instancesize 16, then have the shadermanager put in a define that does the devide?

uniform mat4 unimat40; //mvp
uniform mat4 unimat41; // mv

//uniform vec2 uniscreentodepth;
uniform vec2 tilesize;


in vec3 posattrib;
struct lightdata {
	vec3 pos;
	float size;
};
struct tiledata {
	uvec2 pos;
	uint lcount;
	uint llist[8];
};

#define N 4096
layout (std140) uniform uniblock0 {
	lightdata ldata[N]; //todo
} uniblock0_t;
layout (std140) uniform uniblock1 {
	tiledata tdata[N]; //todo
} uniblock1_t;

out vec3 mvpos; // vertex position in viewspace

#ifdef DIRECTIONAL
	out vec3 lightdirection;
#else
	out vec3 lpos; // light position in viewspace
	out float lsize; // size of light
#endif



void main(){
	vec4 instanceattrib = uniblock0_t.ldata[gl_InstanceID];
	#ifdef DIRECTIONAL
		lightdirection = (unimat41 * vec4(instanceattrib.xyz, 0.0)).xyz;
		//todo figure out a method to get a FSQUAD that also works with the mvpos angle setup
	#else
		vec3 translated = (posattrib * instanceattrib.a) + instanceattrib.rgb;
		lsize = instanceattrib.a;
		lpos = (unimat41 * vec4(instanceattrib.xyz, 1.0)).xyz; // viewspace of light
		//lpos = instanceattrib.xyz;
		mvpos = (unimat41 * vec4(translated, 1.0)).xyz; //viewspace of the mvpos
		gl_Position = unimat40 * vec4(translated, 1.0);
	#endif
}
