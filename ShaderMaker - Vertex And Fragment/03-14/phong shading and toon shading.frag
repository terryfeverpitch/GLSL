//varying vec4 eyepos;
//varying vec3 eyenorm;
varying float ndotl, ndoth;

void main()
{
	vec4 color;
//ambient
	//color = gl_FrontLightProduct[0].ambient;
//diffuse
	//only use in cartoon shading
	float diffuseIntensity;	
	if(ndotl < 0.4)		//set the outline
		diffuseIntensity = 0.0;
	else if(ndotl < 0.6)
		diffuseIntensity = 0.6;
	else if(ndotl < 0.8)
		diffuseIntensity = 0.8;
	else
		diffuseIntensity = 1.0;

	//cartoon shading
	color += diffuseIntensity * gl_FrontLightProduct[0].diffuse;
	//bling phong shading
	//color += ndotl * gl_FrontLightProduct[0].diffuse;
//specular
	//color += pow(ndoth, gl_FrontMaterial.shininess) * gl_FrontLightProduct[0].specular;
	//shininess maximum is 128
	
	gl_FragColor = color;
}
