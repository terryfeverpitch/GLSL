// Phone vertex shader

varying vec3 ecnormal;
varying vec3 ecposition;

void main()
{
	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;

	ecposition = vec3 (gl_ModelViewMatrix * gl_Vertex);
	ecnormal = gl_NormalMatrix * gl_Normal;
}
