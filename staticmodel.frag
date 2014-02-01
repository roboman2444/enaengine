#version 150
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;
in vec3 fragnormal;
in vec3 fragposition;
in vec2 fragtexCoord;


uniform sampler2D texture;

out vec4 fragColor;

void main(){
//	fragColor = vec4(abs(fragposition), 1.0);
	fragColor = vec4(fragnormal, 1.0);
//	fragColor += 0.1;
//	fragColor = vec4(1.0);

//	fragColor = texture2D(texture, fragtexCoord);
}
