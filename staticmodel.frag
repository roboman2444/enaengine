#version 150

//uniform mat4 unimat40;

in vec3 fragnormal;
in vec3 fragposition;
in vec2 fragtexCoord;

#ifdef TEXTURE
	uniform sampler2D texture0;
#endif

out vec4 fragColor;

void main(){
	#ifdef NORMCOLOR
		fragColor = vec4(fragnormal, 1.0);
	#elif defined POSCOLOR
		fragColor = vec4(fragposition, 1.0);
	#elif defined TEXTURE
		fragColor = texture2D(texture0, fragtexCoord);
	//	fragColor = vec4(fragtexCoord, 1.0 , 1.0);

	#else
		fragColor = vec4(1.0);
	#endif
}
