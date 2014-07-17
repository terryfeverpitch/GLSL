#version 120 
#extension GL_EXT_geometry_shader4 : enable

// we got object coordinate as gl_PositionIn IN THIS EXAMPLE ON PURPOSE!!
uniform float time; // in seconds
void main(void)
{

	//Pass-thru!
	for(int i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];
		gl_FrontColor = gl_FrontColorIn[i];
		EmitVertex();
	}
	EndPrimitive();
	
	//New piece of geometry!  We WANT to apply a Z-rotation to the input line.
	float t = radians (6.0*time);  // period was 360sec. 6t to increase freq
	mat4 rotZ = mat4 (cos(t), sin(t), 0., 0., 
	                 -sin(t), cos(t), 0., 0., 
					 0.,          0., 1., 0.,
					 0.,          0., 0., 1.);  // column major
					 
	for(int i=0; i< gl_VerticesIn; i++){
		gl_Position = rotZ * gl_PositionIn[i]; // rotated object coordinates
		gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
		gl_FrontColor = vec4 (1.,0.,0.,1.);   // new lines as red
		EmitVertex();
	}
	EndPrimitive();	
	
	
}
