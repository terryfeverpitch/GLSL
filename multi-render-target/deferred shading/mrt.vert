varying vec3 eyenormal;
varying vec4 eyepos;

void main ()
{
	gl_Position = ftransform();
	eyenormal = gl_NormalMatrix * gl_Normal;
	eyepos = gl_ModelViewMatrix * gl_Vertex;
}
