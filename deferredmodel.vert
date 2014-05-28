#version 150

uniform mat4 unimat40;


in vec3 posattrib;
in vec3 normattrib;
in vec2 tcattrib;

//in mat4 instanceattrib;

#define N 1024 // 16 floats
layout (std140) uniform uniblock0 {
	mat4 ldata[N];
} uniblock0_t;
layout (std140) uniform uniblock1 {
	mat4x3 ldata[N];
} uniblock1_t;
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
	mat4x3 rotmat = uniblock1_t.ldata[gl_InstanceID];

	fragnormal = (rotmat * vec4(normattrib, 1.0)).xyz;
	fragtexCoord = tcattrib;
	fragposition = posattrib;
	#ifdef NORMTEXTURE
		vec4 tvecpre = rotmat * tangentattrib;
		svector = tvecpre.xyz;
		tvector = cross(tvecpre.xyz, fragnormal) * tvecpre.w
	#endif
	gl_Position = instanceattrib * vec4(posattrib, 1.0);
}
