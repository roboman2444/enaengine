//#define VOLUMETRIC
in vec3 mvpos;
in vec2 screenpos;
#ifdef DIRECTIONAL
	flat in vec3 lightnormal;
#else
	#ifdef SPOT
		flat in mat4 lmv;
	#endif
	flat in float lsize;
	flat in vec3 lpos; //viewspace of light
#endif //DIRECTIONAL
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
#endif //MULTISAMPLE

uniform vec2 uniscreensizefix;
uniform vec2 uniscreentodepth;
out vec4 fragColor;

void main(){
	fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	#ifdef MULTISAMPLE
	#ifdef VOLUMETRIC
		float avgz = 0.0f;
		vec3 avgeye = vec3(0.0f);
	#endif //VOLUMETRIC
		ivec2 tc = ivec2(gl_FragCoord.xy);
		int i;
		for(i = 0; i < numsamples; i++){
	#else
		vec2 tc = gl_FragCoord.xy * uniscreensizefix;
	#endif //MULTISAMPLE
	//calculate viewspace pixel position
		//get the geometry information (depth, normal, etc)
		#ifdef MULTISAMPLE
			vec4 normaldist = texelFetch(texture1, tc, i);
		#else
			vec4 normaldist = texture(texture1, tc);
		#endif	//MULTISAMPLE
		vec3 pos;
		pos.z = normaldist.b;
		pos.xy = mvpos.xy * (pos.z / mvpos.z);
//		vec2 screen2pos = (tc *2.0) - vec2(1.0);
//		pos.xy = -screen2pos * (pos.z * uniscreentodepth);
		#ifdef SPOT
			vec4 mtpos = lmv * vec4(pos,1.0);
			vec3 mfpos = mtpos.xyz / mtpos.w;
			float mtlen = length(mfpos.xy);
			if(mtlen > 1.0) discard;
		#endif

		#ifndef DIRECTIONAL
			vec3 lightdelta = lpos-pos;
			float lightdist = length(lightdelta);
//			if(lightdist > lsize) discard;
			vec3 lightnormal = lightdelta/lightdist;
		#endif //DIRECTIONAL
		#ifdef MULTISAMPLE
			vec4 difftex = texelFetch(texture0, tc, i);
		#else
			vec4 difftex = texture(texture0, tc);
		#endif //MULTISAMPLE



		vec3 eyenormal = -normalize(pos);
		vec3 diffuse = difftex.rgb;
		vec2 gloss = vec2(difftex.a, normaldist.a);
		//vec3 surfnormal = normalize(normaldist.rgb);
		vec3 surfnormal = vec3(normaldist.rg, sqrt(1.0f-dot(normaldist.rg, normaldist.rg)));
		vec3 vhalf = normalize(lightnormal+eyenormal);


	//shading
	//	float attenuation = 1.0/(1.0 + lsize * 2 *pow(lightdist, 2));
	//	float attenuation = clamp(1.0 - lightdist/lsize, 0.0, 1.0); attenuation *= attenuation;

		#ifdef DIRECTIONAL
			float attenuation = 1.0f;
		#else
			float attenuation = clamp(1.0f - lightdist*lightdist/(lsize*lsize), 0.0f, 1.0f); attenuation *= attenuation;
			#ifdef SPOT
//				attenuation *= clamp(1.0f - mtlen * mtlen,0.0f, 1.0f);
//				attenuation *= 1.0f;
			#endif
		#endif //DIRECTIONAL


		fragColor.rgb += ((clamp(dot(surfnormal, lightnormal), 0.0f, 1.0f) * diffuse)
		 + vec3(clamp(pow(dot(surfnormal,vhalf), gloss.y), 0.0f, 1.0f) * gloss.x) )* attenuation;
//		fragColor.rgb = pos/16.0;

	#ifdef MULTISAMPLE
		#ifdef VOLUMETRIC
			avgz += pos.z;
			avgeye +=eyenormal;
		#endif //VOLUMETRIC
		}
		#ifdef VOLUMETRIC
			avgz /=numsamples;
			avgeye /= numsamples;
		#endif //VOLUMETRIC
		fragColor.rgb /= numsamples;
	#endif  //MULTISAMPLE




//#define VOLUMETRIC
#ifdef VOLUMETRIC

		//if(lightdist > lsize)
//		0 0 0
//		> t b
//		pos.z -= i /numsamples;
		#ifdef MULTISAMPLE
			vec3 eyenormal = avgeye;
			vec3 pos;
			pos.z = avgz;
		#endif //MULTISAMPLE
		if(lpos.z - lsize > pos.z || pos.z ==0.0)
			pos.z = lpos.z - lsize;

		float delta = (lpos.z + lsize - pos.z) /10.0;
//		float delta = 1.0;
		for(int j = 0; j < 10; j++){
			pos.z = pos.z + delta;
			pos.xy = mvpos.xy * (pos.z / mvpos.z);
			vec3 lightdelta = lpos-pos;
			float lightdist = length(lightdelta);
			float attenuation = clamp(1.0 - lightdist*lightdist/(lsize*lsize), 0.0, 1.0); attenuation *= attenuation;


			vec3 lightnormal = lightdelta/lightdist;
			vec3 halfdir = normalize(lightnormal + eyenormal);
			float h_dot_l = float(dot(halfdir,lightnormal));
			float base = 1.0f - h_dot_l;
			float exponent = pow(base, 5.0);
			fragColor.rgb += attenuation * exponent * 0.2;
		}

#endif //VOLUMETRIC
//			fragColor = vec4(0.1);

	#ifdef SPOT
//		fragColor += vec4(0.1);
	#endif
}
