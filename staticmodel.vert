#version 150

in vec2 texCoord;
in vec3 position;
in vec3 normal;
out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexCoord;
vec3 newpos;


void main(){
	fragnormal = normal;
	fragposition = position;
	fragtexCoord = texCoord;
	newpos = position/3.0;
//	newpos.z/=10.0;
	newpos.y -= 0.5;
	newpos.z += 0.5;
//	newpos.z = 1.0;
	gl_Position = vec4(newpos, 1.0);
//	gl_PointSize = 5.0;
}
