/*void main()
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}*/

varying vec4 objpos;

void main()
{
	gl_Position = ftransform();
	objpos = gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}