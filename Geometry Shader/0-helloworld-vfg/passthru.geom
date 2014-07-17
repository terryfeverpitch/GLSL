#version 120 
#extension GL_EXT_geometry_shader4 : enable

void main(void)
{
	//Pass-thru!
	for(int i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_PositionIn[i];
	//	gl_Position.xy = gl_Position.yx;
		gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
		gl_FrontColor = gl_FrontColorIn[i];
		gl_TexCoord[0] = gl_TexCoordIn[i][0];
		EmitVertex();
	}
	EndPrimitive();
	
	//Pass-thru!
	for(int i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_PositionIn[i];
		gl_Position.xy = gl_Position.xy;
		gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
		gl_FrontColor = gl_FrontColorIn[i];
		gl_TexCoord[0] = gl_TexCoordIn[i][0];
		EmitVertex();
	}
}