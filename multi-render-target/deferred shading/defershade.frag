uniform sampler2D img_diffuse;
uniform sampler2D img_normal;
uniform sampler2D img_depth;

void main()
{
	if (texture2D (img_depth, gl_TexCoord[0].st).r > 0.996) 
		gl_FragColor = vec4 (vec3(1.0), 1.0);
	else {
		vec3 normal = texture2D (img_normal, gl_TexCoord[0].st).xyz;

		normal = (normal - vec3(0.5,0.5,0.5))*2.0;
		float intensity = dot (normal, normalize (gl_LightSource[0].position.xyz));
		
		gl_FragColor = intensity*texture2D (img_diffuse, gl_TexCoord[0].st);
	}
}
