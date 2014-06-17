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
		fragColor = texture(texture0, fragtexCoord);
	#else
		fragColor = vec4(1.0);
	#endif
	#ifdef LIGHT
		vec3 lightnormal = normalize(vec3(1.0, 1.0, 1.0));
		float n_dot_l = clamp(float(dot(fragnormal, lightnormal)), 0.0, 1.0);
		fragColor*= n_dot_l;
	#endif
}
