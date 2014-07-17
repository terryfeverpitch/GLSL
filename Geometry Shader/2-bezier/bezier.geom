#version 120 
#extension GL_EXT_geometry_shader4 : enable

uniform int Num;

void main(void)
{
	//increment variable
	int i;
	float dt = 1.0/float(Num);
	float t = 0.0;
	for (i = 0; i <= Num; i++) {
		float omt = 1. - t;
		float omt2 = omt*omt;
		float omt3 = omt2*omt;
		float t2 = t*t;
		float t3 = t2*t;
		
		vec4 pt = omt3 * gl_PositionIn[0] + 3*t*omt2* gl_PositionIn[1]
		         + 3*t2*omt* gl_PositionIn[2] + t3* gl_PositionIn[3];
		gl_Position = pt;
		EmitVertex();
		t += dt;
	}
}
