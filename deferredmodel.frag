#version 150

uniform mat4 unimat40;
uniform mat4 unimat41;

in vec3 fragnormal;
in vec3 fragposition;
in vec2 fragtexCoord;

#ifdef DIFFTEXTURE
	uniform sampler2D texture0;
#endif
#ifdef NORMTEXTURE
//	in vec4 fragtangent;
	in vec3 tvector;
	in vec3 svector;
	uniform sampler2D texture1;
#endif
#ifdef SPECTEXTURE
	uniform sampler2D texture3;
#endif

out vec4 fragColor;
#ifdef DEFERRED
out vec4 normColor;
out vec4 specColor;
#endif

void main(){
	#ifdef DIFFTEXTURE
		vec3 dcolor = texture2D(texture0, fragtexCoord).rgb;
	#else
		vec3 dcolor = vec3(1.0);
	#endif
	#ifdef NORMTEXTURE
		vec3 texnormal = texture2D(texture1, fragtexCoord).rgb;
		vec3 ncolor = texnormal;
		vec3 ncolor = normalize(texnormal.x * vertexsvector + texnormal.y * vertextvector + texnormal.z * vertexnormal);
	#else
		vec3 ncolor = normalize(fragnormal);
	#endif
	#ifdef SPECTEXTURE
		vec2 scolor = texture2D(texture3, fragtexCoord).rg;
	#else
		vec2 scolor = vec2(0.5);
	#endif

	fragColor = vec4(dcolor,1.0);
	fragColor.rgb = fragnormal;

	#ifdef NORMTEXTURE
		fragColor = tvector;
	#endif
	#ifdef DEFERRED
		normColor.rgb = ncolor;
		normColor.a = fragposition.z;
		specColor.rg = scolor;
	#endif
}
