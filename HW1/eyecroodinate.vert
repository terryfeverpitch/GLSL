// simple vertex shader
uniform int objeye;
varying vec4 eyePos;

void main()
{
	if( objeye == 1 )
	{
		gl_Position = ftransform();
		eyePos = gl_Vertex;
	}
	else
	{
		gl_Position = ftransform();
		eyePos = gl_ModelViewMatrix * gl_Vertex;
	}
		
}
