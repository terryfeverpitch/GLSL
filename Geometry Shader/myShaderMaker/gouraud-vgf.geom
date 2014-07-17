// simple geometry shader

// these lines enable the geometry shader support.
#version 120
#extension GL_EXT_geometry_shader4 : enable

varying in float ndotl[3];
varying out float ndotl_out;

void main( void )
{
	for( int i = 0 ; i < gl_VerticesIn ; i++ )
	{
		gl_Position    = gl_PositionIn  [ i ];
		ndotl_out = ndotl[i];
		EmitVertex();
	}
}
