uniform mat4 unimat40;


in vec3 posattrib;
in vec3 normattrib;
in vec2 tcattrib; // might be able to remove


struct modeldata {
	mat4 modelviewprojection;
};

#define N 512 // 32 floats //setting this to 16 or something low doesnt seem to make a difference...
layout (std140) uniform uniblock0 {
	modeldata data[N];
} uniblock0_t;

void main(){
	modeldata mydata = uniblock0_t.data[gl_InstanceID];
	mat4 mvpmat = mydata.modelviewprojection;
	gl_Position = mvpmat * vec4(posattrib, 1.0);
}
