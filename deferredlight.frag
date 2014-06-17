#version 150

//uniform mat4 unimat40;
//vec2(Far / (Far - Near), Far * Near / (Near - Far));
uniform vec2 uniscreentodepth;

in vec3 lpos; // worldspace of light
in vec3 mvpos;
in float lsize;
//in vec2 fragtexcoord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec2 uniscreensizefix;

out vec4 fragColor;

void main(){
		//calculate viewspace pixel position
		vec2 tc = gl_FragCoord.xy * uniscreensizefix;
		vec3 pos;
		//get the geometry information (depth, normal)
		vec4 normaldist = texture(texture1, tc);
//		pos.z = normaldist.a;
		pos.z = uniscreentodepth.y / normaldist.a + uniscreentodepth.x;
		pos.xy = mvpos.xy * (pos.z / mvpos.z);
//		pos.xy = mvpos.xy * (pos.z / (uniscreentodepth.y / mvpos.z + uniscreentodepth.x));

		vec3 eyenormal = -normalize(pos);
		vec3 lightdelta = lpos-pos;
		float lightdist = length(lightdelta);
		vec3 lightnormal = normalize(lightdelta);
		vec3 surfnormal = normaldist.rgb;

//		float diffuse = clamp(dot(surfnormal, lightnormal * 2.0), 0.0, 1.0);
//		fragColor = diffuse*texture(texture0, tc);

		vec2 gloss = texture(texture2, tc).rg;
		vec3 diffuse = texture(texture0, tc).rgb;
/*
		float SpecularPower = 32.0;
		float specpowergloss = gloss.g * SpecularPower;
		float n_dot_l = clamp(float(dot(surfnormal, lightnormal)), 0.0, 1.0);
		float n_dot_v = clamp(float(dot(surfnormal, eyenormal)), 0.0, 1.0);
		vec3 half = normalize(eyenormal + lightnormal);
		float n_dot_h = clamp(float(dot(surfnormal,half)), 0.0, 1.0);
		float h_dot_l = float(dot(half,lightnormal));


		float h_dot_v = clamp(float(dot(half, eyenormal)), 0.0, 1.0); // muh


		float normalisation_term = (specpowergloss+2.0f)/8.0f;
		float blinn_phong = pow(n_dot_h, specpowergloss);
		float specular_term = normalisation_term * blinn_phong;

		float cosine_term = n_dot_l;

		float base = 1.0f - h_dot_l;
		float exponential = pow(base, 5.0);
		float fresnel_term = gloss.r  + (( 1.0f - gloss.r ) * exponential);

//	float visibility_term = 1.0; // setting this to 1.0 gives an extremly slight perf boost
	#define PI_OVER_FOUR 0.78539816339
	#define PI_OVER_TWO 1.57079632679
	float alpha = 1.0f / ( sqrt( PI_OVER_FOUR * specpowergloss + PI_OVER_TWO ) );
	float visibility_term = ( n_dot_l * ( 1.0f - alpha ) + alpha ) * ( n_dot_v * ( 1.0f - alpha ) + alpha ); // Both dot products should be saturated
	visibility_term = 1.0f / visibility_term;

	float specular = specular_term * cosine_term * fresnel_term * visibility_term;
	vec3 specfinal = vec3(specular);// * Color_Specular;

 	float Fdiff = gloss.r + (1.0 - gloss.r) * pow(1.0 - n_dot_l, 5.0);

	vec3 difffinal = diffuse; //* Color_Diffuse * Color_Diffuse;
//	color.rgb = diffuse * diffusetex * Color_Diffuse + specfinal;
	fragColor.rgb = difffinal * (1.0- Fdiff * 0.5);// + specfinal;

*/
//	float attenuation = 1.0/(1.0 + lsize * 2 *pow(lightdist, 2));
//	float attenuation = clamp(1.0 - lightdist/lsize, 0.0, 1.0); attenuation *= attenuation;
	float attenuation = clamp(1.0 - lightdist*lightdist/(lsize*lsize), 0.0, 1.0); attenuation *= attenuation;

	fragColor.rgb = clamp(dot(surfnormal, lightnormal), 0.0, 1.0) * diffuse *attenuation;
	fragColor.a = 1.0;


}
