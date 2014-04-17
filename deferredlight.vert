#version 150

uniform mat4 unimat40;


in vec3 posattrib;
//in vec3 normattrib;
//in vec2 tcattrib;
//out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexCoord;


void main(){
//	fragnormal = normattrib;
//	fragtexCoord = tcattrib;
//	fragposition = posattrib;
	gl_Position = unimat40 * vec4(posattrib, 1.0);
	fragposition = vec3(gl_Position);
//	fragtexCoord = (vec2(gl_Position) + vec2(1.0)) * 0.5;
	fragtexCoord = vec2(gl_Position);
}
