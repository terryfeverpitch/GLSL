#version 120 
#extension GL_EXT_geometry_shader4 : enable

//CIRL GPU Geometry Program: Derek Anderson and Robert Luke
// very simple geometry shader
void main(void)
{
	
	//increment variable
	int i;

	//Pass-thru!
	for(i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();
	
	//New piece of geometry!  We just swizzle the x and y terms
	for(i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_PositionIn[i];
		gl_Position.xy = gl_Position.yx;
		gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
		EmitVertex();
	}
	EndPrimitive();	
	
	/////////////////////////////////////////////////////////////
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
