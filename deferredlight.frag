#version 150

//uniform mat4 unimat40;

in vec3 fragposition;
in vec2 fragtexCoord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

out vec4 fragColor;

void main(){
		fragColor = vec4(0.1);
//		fragColor = vec4(fragposition, 1.0);
//		fragColor = texture2D(texture0, fragposition.xy);
		fragColor += texture2D(texture0, fragtexCoord);
//		fragColor += texture2D(texture1, fragtexCoord) * 2.0;
//		fragColor += texture2D(texture2, fragtexCoord) * 2.0;
}
