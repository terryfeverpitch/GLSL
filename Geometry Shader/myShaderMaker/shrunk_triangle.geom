// simple geometry shader

// these lines enable the geometry shader support.
#version 120
#extension GL_EXT_geometry_shader4 : enable

vec3 CG;
vec3 v[3];
uniform float shrink;

void
ProduceVertex(int i)
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(CG + shrink*(v[i]-CG), 1.0);
	EmitVertex();
}


void main( void )
{
	v[0] = gl_PositionIn[0].xyz;
	v[1] = gl_PositionIn[1].xyz;
	v[2] = gl_PositionIn[2].xyz;

	CG = (v[0] + v[1] + v[2])/3.;
	ProduceVertex(0);
	ProduceVertex(1);
	ProduceVertex(2);
}
