varying vec4 ecposition;
varying vec3 normal;

void main ()
{
	ecposition = gl_ModelViewMatrix * gl_Vertex;
	normal = gl_NormalMatrix * gl_Normal;
	gl_Position = ftransform();
}
