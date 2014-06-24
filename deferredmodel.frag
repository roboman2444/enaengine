#version 150

uniform mat4 unimat40;
uniform mat4 unimat41;

in vec3 fragnormal;
in vec3 fragposition;
in vec2 fragtexCoord;
in float depth;
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
out vec4 normColor;
out vec4 specColor;

void main(){
	#ifdef DIFFTEXTURE
		vec3 dcolor = texture(texture0, fragtexCoord).rgb;
	#else
		vec3 dcolor = vec3(1.0);
//		vec3 dcolor = vec3(sin(fragposition.x), sin(fragposition.y), sin(fragposition.z));
	#endif
	#ifdef NORMTEXTURE
		vec3 texnormal = texture(texture1, fragtexCoord).rgb - vec3(0.5); //maybe regen a component using a cross
		//vec3 ncolor = texnormal;
		vec3 ncolor = normalize(texnormal.x * svector + texnormal.y * tvector + texnormal.z * fragnormal);
	#else
		vec3 ncolor = fragnormal;
	#endif
	#ifdef SPECTEXTURE
		vec2 scolor = texture(texture3, fragtexCoord).rg;
			scolor.g *= 256;
	#else
		vec2 scolor = vec2(1.0, 2000.0);
	#endif

	fragColor = vec4(dcolor, scolor.r);
//	fragColor.rgb = fragnormal;

	normColor.rg = ncolor.rg;
//	normColor.a = fragposition.z;
	normColor.b = depth;
	normColor.a = scolor.g;
//	normColor.a = length(fragposition.xyz);
//	specColor.rg = scolor;
}
