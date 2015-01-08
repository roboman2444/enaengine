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
		vec2 scolor = vec2(0.5, 50.0);
	#endif

	fragColor = vec4(dcolor, scolor.r);
	normColor = vec4(ncolor.rg, depth, scolor.g);
	




	//do per material shading here
	//rgb is addative premultiplied
	//a is alpha
	//example for ambient with a distance darkening
	//usually a fog/distance darkening would be done in another pass, so you dont have any problems against stuff that isnt drawn, but this is an example to what can be done
	//another good use for this might be a cubemap reflection shader, subsurface scattering, etc
	vec3 add = 0.1*dcolor.rgb; // ambient
	float alpha = 1.0-clamp((500.0+depth)/(500.0-10.0), 0.0, 1.0);
//	add *=(1.0-alpha); //needs the premult here, since i dont do it later
//	add += alpha;
	specColor = vec4(add, alpha);
//	specColor = vec4(0.0, add), 0.1);
}
