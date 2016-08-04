uniform mat4 unimat40;


in vec3 posattrib;

struct modeldata {
	mat4 modelviewprojection;
	vec3 color;
};

#define N 512 // 32 floats //setting this to 16 or something low doesnt seem to make a difference...
layout (std140) uniform uniblock0 {
	modeldata data[N];
} uniblock0_t;

out vec3 color;
void main(){
	modeldata mydata = uniblock0_t.data[gl_InstanceID];
	mat4 mvpmat = mydata.modelviewprojection;
	color = mydata.color;
	gl_Position = mvpmat * vec4(posattrib, 1.0);
//	gl_Position =  vec4(posattrib, 1.0);
}
