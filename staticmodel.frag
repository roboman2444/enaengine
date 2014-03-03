#version 150

//uniform mat4 unimat40;

in vec3 fragnormal;
in vec3 fragposition;
in vec2 fragtexCoord;

#ifdef TEXTURE
	uniform sampler2D texture0;
	uniform sampler2D texture1;
	uniform sampler2D texture2;
	uniform sampler2D texture3;
#endif

out vec4 fragColor;

void main(){
	#ifdef NORMCOLOR
		fragColor = vec4(fragnormal, 1.0);
	#elif defined POSCOLOR
		fragColor = vec4(fragposition, 1.0);
	#elif defined TEXTURE
		fragColor = texture2D(texture0, fragtexCoord);
		fragColor += texture2D(texture2, fragtexCoord);
		fragColor += texture2D(texture3, fragtexCoord);
//		fragColor = texture(texture0, vec2(0.0,0.0)).r * vec4(1.0, 1.0, 1.0, 1.0);
//		fragColor += texture2D(texture1, fragtexCoord + 0.5);
	//	fragColor = vec4(fragtexCoord, 1.0 , 1.0);

	#else
		fragColor = vec4(1.0);
	#endif
}
