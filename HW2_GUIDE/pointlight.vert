// simple vertex shader
varying vec4 eyepos;
varying vec3 eyenorm;
varying vec4 texCoordinate;

void main()
{
	gl_Position    = ftransform();
    eyepos = gl_ModelViewMatrix * gl_Vertex;
	eyenorm = gl_NormalMatrix * gl_Normal;
	texCoordinate = gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
