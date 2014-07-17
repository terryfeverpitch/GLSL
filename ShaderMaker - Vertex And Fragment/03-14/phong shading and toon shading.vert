// simple vertex shader
//varying vec4 eyepos;
//varying vec3 eyenorm;
varying float ndotl, ndoth;

void main()
{
	gl_Position = ftransform();
	vec4 eyepos = gl_ModelViewMatrix * gl_Vertex;
	vec3 eyenorm = gl_NormalMatrix * gl_Normal;
	vec3 L = gl_LightSource[0].position.xyz - vec3(eyepos); //point light
	//vec3 lightpos = gl_LightSource[0].position.xyz;//directional light
	ndotl = dot(normalize(eyenorm), normalize(L));

	vec3 V = -vec3(eyepos);	
	vec3 H = V+L;
	ndoth = dot(normalize(eyenorm), normalize(H));

}
