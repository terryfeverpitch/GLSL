// simple vertex shader

void main()
{
	gl_Position    = ftransform();
	vec4 eyepos = gl_ModelViewMatrix * gl_Vertex;
	vec3 eyenorm = gl_NormalMatrix * gl_Normal;

	vec3 light = gl_LightSource[0].position.xyz - vec3(eyepos);
	float ndotl = dot (normalize (eyenorm),
       normalize(light));
	vec3 halfV = - normalize(vec3(eyepos)) + normalize(light); 
	float ndoth = dot (normalize (eyenorm), 
       normalize (halfV));
 
	vec4 colorsum;
	colorsum = gl_FrontLightProduct[0].ambient;
	colorsum += ndotl * gl_FrontLightProduct[0].diffuse;
	colorsum += pow (ndoth, gl_FrontMaterial.shininess) 
       * gl_FrontLightProduct[0].specular;

	gl_FrontColor = colorsum;
}
