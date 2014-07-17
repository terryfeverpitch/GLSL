// bumpmap

varying vec3 lightDir;
uniform sampler2D NormalMap;
uniform sampler2D ColorMap;

void main()
{
	// fetch normal from normal map
	vec3 norm = vec3 (texture2D (NormalMap, gl_TexCoord[0].st));
	norm = (norm - 0.5)*2.0;
	float intensity = dot(normalize(lightDir), norm);
	//float intensity = dot((lightDir), norm);
	vec4 color = intensity*vec4(1.0, 1.0, 1.0, 1.0);//*texture2D (ColorMap, gl_TexCoord[0].st);
	gl_FragColor = color;
}
