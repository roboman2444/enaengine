#version 150

//uniform mat4 unimat40;

in vec3 fragnormal;
in vec3 fragposition;
in vec2 fragtexCoord;

uniform sampler2D texture;

out vec4 fragColor;

void main(){
	#ifdef NORMCOLOR
	fragColor = vec4(fragnormal, 1.0);
//		AGknjnf
	#else
		#ifdef POSCOLOR
			fragColor = vec4(fragposition, 1.0);

		#else
			fragColor = vec4(1.0);
		#endif
	#endif
//	fragColor = vec4(1.0);
}
