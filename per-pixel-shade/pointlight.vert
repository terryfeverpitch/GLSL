// simple vertex shader
varying vec4 eyepos;
varying vec3 eyenorm;

void main()
{
	gl_Position    = ftransform();
    eyepos = gl_ModelViewMatrix * gl_Vertex;
	eyenorm = gl_NormalMatrix * gl_Normal;
}
