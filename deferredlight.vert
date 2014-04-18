#version 150

uniform mat4 unimat40;


in vec3 posattrib;
//in vec3 normattrib;
//in vec2 tcattrib;
//out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexcoord;


void main(){
//	fragnormal = normattrib;
//	fragtexcoord = tcattrib;
//	fragposition = posattrib;
	gl_Position = unimat40 * vec4(posattrib, 1.0);
	fragposition = gl_Position.xyz;
//	fragtexcoord = (gl_Position.xy /gl_Position.w );
//	fragtexcoord = vec2(gl_Position);
}
