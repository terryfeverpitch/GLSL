// simplest vertex shader

void main()
{	
	//gl_Position = ftransform();
	gl_Position = gl_Vertex;
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
