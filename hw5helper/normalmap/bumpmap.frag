// bumpmap

varying vec3 lightDir;
uniform sampler2D NormalMap;
uniform sampler2D ColorMap;

uniform bool nobump;

void main()
{
	// fetch normal from normal map
	vec3 norm = vec3 (texture2D (NormalMap, gl_TexCoord[0].st));
	norm = (norm - 0.5)*2.0;
	float intensity = dot (lightDir, norm);

	if (nobump)
		intensity = lightDir.z; //dot (lightDir, vec3(0,0,1));
		
	vec4 color = intensity*texture2D (ColorMap, gl_TexCoord[0].st);
	gl_FragColor = color;
}
