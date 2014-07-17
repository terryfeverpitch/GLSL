// simple geometry shader

// these lines enable the geometry shader support.
#version 120
#extension GL_EXT_geometry_shader4 : enable
varying in vec3 ecposition[3];
varying in vec3 ecnormal[3];
varying out vec3 ecposition_out;
varying out vec3 ecnormal_out;

void main( void )
{
	for( int i = 0 ; i < gl_VerticesIn ; i++ )
	{
		gl_Position    = gl_PositionIn  [ i ];
		ecposition_out = ecposition[i];
		ecnormal_out = ecnormal[i];
		EmitVertex();
	}
}
