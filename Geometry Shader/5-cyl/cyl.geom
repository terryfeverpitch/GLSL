#version 120 
#extension GL_EXT_geometry_shader4 : enable

uniform int segs;
varying out float lightIntensity;  // one per vertex
uniform float radius;

vec3 rodriguesRot (vec3 v, vec3 axis, float rad)
{
	return v*cos(rad) + cross(axis,v)*sin(rad) + axis*dot(axis,v)*(1-cos(rad));
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
	return radius*normalize(basis[min] - dot(axis,basis[min])*axis);
}

void main(void)
{
	vec3 axis = normalize (vec3(gl_PositionIn[1]-gl_PositionIn[0]));
	vec3 span = getSpan (axis);
	
	int i, j;

	//Pass-thru!
	for(int i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();

	vec3 ecnormal;
	vec3 lightpos = gl_LightSource[0].position.xyz;  // eyespace
	vec4 pos;
	vec3 turned_span;
	
	vec3 delta_spine = (gl_PositionIn[1].xyz-gl_PositionIn[0].xyz)/float(segs);
	for (i = 0; i < segs; i++) {
		vec3 spine = gl_PositionIn[0].xyz + float(i)*delta_spine;

		for (j = 0; j <= 12; j++) {
			turned_span = rodriguesRot(span, axis, radians(float(j)*360./float(12)));
			pos = vec4 (spine + turned_span, 1.0);
			vec3 lightvec = normalize (lightpos - vec3(gl_ModelViewMatrix*pos));  // eye space
			lightIntensity = max (0.0, dot(gl_NormalMatrix*normalize(turned_span), lightvec));
			gl_Position = gl_ModelViewProjectionMatrix * pos;
			EmitVertex();
			
			pos = vec4 (spine + delta_spine + turned_span, 1.0);
			lightvec = normalize (lightpos - vec3(gl_ModelViewMatrix*pos));
			lightIntensity = max (0.0, dot(gl_NormalMatrix*normalize(turned_span), lightvec));
			gl_Position = gl_ModelViewProjectionMatrix * pos;
			EmitVertex();
		}

		EndPrimitive();
	}
}