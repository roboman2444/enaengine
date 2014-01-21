#version 150
in vec3 fragnormal;
in vec3 fragposition;
in vec2 fragtexCoord;

out vec4 fragColor;

void main(){
	fragColor = vec4(abs(fragposition), 1.0);
//	fragColor = vec4(1.0);
}
