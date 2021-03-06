uniform mat4 unimat40;


in vec3 posattrib;
in vec3 normattrib;
in vec2 tcattrib;


struct modeldata {
	mat4 modelviewprojection;
	mat4 modelview;
};

#define N 512 // 32 floats //setting this to 16 or something low doesnt seem to make a difference...
layout (std140) uniform uniblock0 {
	modeldata data[N];
} uniblock0_t;

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
	modeldata mydata = uniblock0_t.data[gl_InstanceID];
	mat4 mvpmat = mydata.modelviewprojection;
	mat4 mvmat = mydata.modelview;

	fragnormal = normalize((mvmat * vec4(normattrib, 0.0)).xyz);
	fragtexCoord = tcattrib;
	#ifdef NORMTEXTURE
		//the 0.0 cancles out the move and crap... its odd
	//	vec3 tvecpre = (mvmat * vec4(tangentattrib.xyz, 0.0)).xyz;
		vec3 tvecpre = normalize((mvmat * vec4(tangentattrib.xyz, 0.0)).xyz);
		svector = tvecpre.xyz;
		tvector = cross(tvecpre.xyz, fragnormal) * tangentattrib.w;
	#endif
	gl_Position = mvpmat * vec4(posattrib, 1.0);
	fragposition = gl_Position.xyz;
	depth = (mvmat * vec4(posattrib, 1.0)).z;
}
