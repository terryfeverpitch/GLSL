// simple vertex shader

varying float ndotl;

void main()
{
	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;

	vec3 ecnormal = gl_NormalMatrix * gl_Normal;
	vec4 ecposition = gl_ModelViewMatrix * gl_Vertex;
	vec3 l = normalize(gl_LightSource[0].position.xyz - ecposition.xyz);
	ndotl = max (0.0, dot(ecnormal, l));
}
