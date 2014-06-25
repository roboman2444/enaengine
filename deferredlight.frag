#version 150

//uniform mat4 unimat40;
//vec2(Far / (Far - Near), Far * Near / (Near - Far));
uniform vec2 uniscreentodepth;
in vec3 mvpos;
#ifdef DIRECTIONAL
in vec3 lightnormal;
#else
	in float lsize;
	in vec3 lpos; //viewspace of light
#endif
#ifdef MULTISAMPLE
	#define numsamples uniint0
	uniform int numsamples;
	uniform sampler2DMS texture0;
	uniform sampler2DMS texture1;
	uniform sampler2DMS texture2;
	uniform sampler2DMS texture3;
#else
	uniform sampler2D texture0;
	uniform sampler2D texture1;
	uniform sampler2D texture2;
	uniform sampler2D texture3;
#endif
uniform vec2 uniscreensizefix;

out vec4 fragColor;

void main(){
	fragColor.rgba = vec4(0.0, 0.0, 0.0, 1.0);
	#ifdef MULTISAMPLE
		ivec2 tc = ivec2(gl_FragCoord.xy);
		int i;
		for(i = 0; i < numsamples; i++){
	#else
		vec2 tc = gl_FragCoord.xy * uniscreensizefix;
	#endif
	//calculate viewspace pixel position
		//get the geometry information (depth, normal, etc)
		#ifdef MULTISAMPLE
			vec4 normaldist = texelFetch(texture1, tc, i);
//			vec2 gloss = texelFetch(texture2, tc, i).rg;
			vec4 difftex = texelFetch(texture0, tc, i);
		#else
			vec4 normaldist = texture(texture1, tc);
//			vec2 gloss = texture(texture2, tc).rg;
			vec4 difftex = texture(texture0, tc);
		#endif
		vec3 diffuse = difftex.rgb;
		vec2 gloss = vec2(difftex.a, normaldist.a);
		vec3 pos;
		pos.z = normaldist.b;
		pos.xy = mvpos.xy * (pos.z / mvpos.z);
		vec3 eyenormal = -normalize(pos);
		#ifndef DIRECTIONAL
			vec3 lightdelta = lpos-pos;
			float lightdist = length(lightdelta);
			vec3 lightnormal = lightdelta/lightdist;
		#endif
		//vec3 surfnormal = normalize(normaldist.rgb);
		vec3 surfnormal = vec3(normaldist.rg, sqrt(1.0-dot(normaldist.rg, normaldist.rg)));
		vec3 vhalf = normalize(lightnormal+eyenormal);


	//shading
	//	float attenuation = 1.0/(1.0 + lsize * 2 *pow(lightdist, 2));
	//	float attenuation = clamp(1.0 - lightdist/lsize, 0.0, 1.0); attenuation *= attenuation;

		#ifdef DIRECTIONAL
			float attenuation = 1.0;
		#else
			float attenuation = clamp(1.0 - lightdist*lightdist/(lsize*lsize), 0.0, 1.0); attenuation *= attenuation;
		#endif

		fragColor.rgb += clamp(dot(surfnormal, lightnormal), 0.0, 1.0) * diffuse * attenuation;
		fragColor.rgb += vec3(clamp(pow(dot(surfnormal,vhalf), gloss.y), 0.0, 1.0) * attenuation * gloss.x);




	#ifdef MULTISAMPLE
		}
		fragColor.rgb /= numsamples;
	#endif

}
