#version 150
//todo put something in shadermanager to handle this
//something like a define instancesize 16, then have the shadermanager put in a define that does the devide?

uniform mat4 unimat40; //mvp
uniform mat4 unimat41; // mv

//uniform vec2 uniscreentodepth;


in vec3 posattrib;

#define N 4096
layout (std140) uniform uniblock0 {
	vec4 ldata[N];
} uniblock0_t;
out vec3 lpos; // light position in viewspace
out vec3 mvpos; // vertex position in viewspace
out float lsize; // size of light


void main(){
	vec4 instanceattrib = uniblock0_t.ldata[gl_InstanceID];
	vec3 translated = (posattrib * instanceattrib.a) + instanceattrib.rgb;
	lsize = instanceattrib.a;
	lpos = (unimat41 * vec4(instanceattrib.xyz, 1.0)).xyz; // viewspace of light
	//lpos = instanceattrib.xyz;
	mvpos = (unimat41 * vec4(translated, 1.0)).xyz; //viewspace of the mvpos
	gl_Position = unimat40 * vec4(translated, 1.0);
}
