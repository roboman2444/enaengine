#version 150

uniform mat4 unimat40;
uniform mat4 unimat41;


in vec3 posattrib;
in vec3 normattrib;
in vec2 tcattrib;

//in mat4 instanceattrib;

#define N 1024
layout (std140) uniform uniblock0 {
	mat4 ldata[N];
} uniblock0_t;
out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexCoord;
#ifdef NORMTEXTURE
	in vec4 tangentattrib;
	out vec3 tvector;
	out vec3 svector;
#endif


void main(){
	mat4 instanceattrib = uniblock0_t.ldata[gl_InstanceID];
	fragnormal = (unimat41 * vec4(normattrib, 1.0)).xyz;
	fragtexCoord = tcattrib;
	fragposition = posattrib;
	#ifdef NORMTEXTURE
		vec4 tvecpre = unimat41 * tangentattrib;
		svector = tvecpre.xyz;
		tvector = cross(tvecpre.xyz, fragnormal) * tvecpre.w
	#endif
	gl_Position = instanceattrib * vec4(posattrib, 1.0);
}
