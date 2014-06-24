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
//	mat3x4 ldata[N];
	mat4 ldata[N];
} uniblock1_t;
out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexCoord;
out float depth;
#ifdef NORMTEXTURE
	in vec4 tangentattrib;
	out vec3 tvector;
	out vec3 svector;
#endif


void main(){
	mat4 instanceattrib = uniblock0_t.ldata[gl_InstanceID];
	mat4 mvmat = uniblock1_t.ldata[gl_InstanceID];

	fragnormal = normalize((mvmat * vec4(normattrib, 0.0)).xyz);
	fragtexCoord = tcattrib;
	#ifdef NORMTEXTURE
		//the 0.0 cancles out the move and crap... its odd
	//	vec3 tvecpre = (mvmat * vec4(tangentattrib.xyz, 0.0)).xyz;
		vec3 tvecpre = normalize((mvmat * vec4(tangentattrib.xyz, 0.0)).xyz);
		svector = tvecpre.xyz;
		tvector = cross(tvecpre.xyz, fragnormal) * tangentattrib.w;
	#endif
	gl_Position = instanceattrib * vec4(posattrib, 1.0);
	fragposition = gl_Position.xyz;
	depth = (mvmat * vec4(posattrib, 1.0)).z;
}
