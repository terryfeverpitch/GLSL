// very simple vertex shader

void main()
{	
	// Transform the vertex (ModelViewProj matrix)
	// postponed till geometry shader (for this application)
	gl_FrontColor = gl_Color;
	gl_Position = gl_Vertex;
}
