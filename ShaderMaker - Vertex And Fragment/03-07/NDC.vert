// simple vertex shader

varying vec4 clip_pos;
void main()
{
	gl_Position = ftransform();
	clip_pos = gl_ModelViewProjectionMatrix * gl_Vertex;
}
