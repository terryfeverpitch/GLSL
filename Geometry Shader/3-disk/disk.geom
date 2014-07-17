#version 120 
#extension GL_EXT_geometry_shader4 : enable

//CIRL GPU Geometry Program: Derek Anderson and Robert Luke
// very simple geometry shader
// INPUT: LINES
// OUTPUT: LINESTRIP
uniform float radius;

vec3 rodriguesRot (vec3 v, vec3 axis, float rad)
{
	return v*cos(rad) + cross(axis,v)*sin(rad) + axis*dot(axis,v)*(1.0-cos(rad));
}
vec3 getSpan(vec3 axis)
{
	// find a basis with least projection, then gram-schmidt to get perperdicular axis
	int min;
	float minproj = 10;
	vec3 basis[3];
	basis[0] = vec3(1,0,0); basis[1] = vec3(0,1,0); basis[2] = vec3(0,0,1);
	for (int i = 0; i < 3; i++) {
		float m;
		if ((m=abs(dot (axis, basis[i]))) < minproj) {
			minproj = m;
			min = i;
		}
	}
	return radius* normalize(basis[min] - dot(axis,basis[min])*axis);
}

void main(void)               
{
	//Pass-thru!
	for(int i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();

	vec3 midpt = (gl_PositionIn[0].xyz + gl_PositionIn[1].xyz)/2.;
	vec3 axis = normalize (gl_PositionIn[1].xyz - gl_PositionIn[0].xyz);
	vec3 span = getSpan(axis); 
	
	// generate a disk in the middle
	for(int i=0; i<= 12; i++){
		float rad = radians(float(i)*360./float(12));
		vec3 add = rodriguesRot (span, axis, rad);
		vec4 pos = vec4 (midpt + add, 1.0);
		gl_Position = gl_ModelViewProjectionMatrix * pos;

		EmitVertex();
	}
	EndPrimitive();	
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
