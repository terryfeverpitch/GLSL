// simple vertex shader

varying vec4 eyePos;

void main()
{
	gl_Position = ftransform();
	//eyePos = gl_ModelViewMatrix * gl_Vertex;	
}
