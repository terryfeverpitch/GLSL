#version 120 
#extension GL_EXT_geometry_shader4 : enable

uniform int Num;

void main(void)
{
	//increment variable
	int i;
	float ds = 1.0/float(Num);
	float s = 0.0;
	for (i = 0; i <= Num; i++) {
		vec3 p1, p2, t1, t2;
		p1 = gl_PositionIn[1].xyz;
		p2 = gl_PositionIn[2].xyz;
		t1 = gl_PositionIn[1].xyz - gl_PositionIn[0].xyz;
		t2 = gl_PositionIn[3].xyz - gl_PositionIn[2].xyz;
		float s2, s3;
		s2 = s*s;
		s3 = s2*s;
		float h1,h2,h3,h4;
		h1 = 2*s3 -3*s2 + 1;
		h2 = -2*s3 + 3*s2;
		h3 = s3 -2*s2 + s;
		h4 = s3 - s2;
		vec3 p = p1*h1 + p2*h2 + t1*h3 + t2*h4;
		gl_Position = gl_ModelViewProjectionMatrix* vec4(p,1.);
		EmitVertex();
		s += ds;
	}
}
