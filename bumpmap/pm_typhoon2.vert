// textbook normal map

// vertex attributes (in object space)
attribute vec3 Tangent;
attribute vec3 Bitangent;
varying vec3 eyeVec;
varying vec3 lightVec;

void main (void)
{
	// do the standard vertex stuff
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;

	// camera position in object space
	// 
	vec4 camera_pos = gl_ModelViewMatrixInverse * vec4 (0.,0.,0.,1.);
	// view vector in object space
	vec3 view = normalize ((camera_pos - gl_Vertex).xyz);

	vec4 light_pos = gl_ModelViewMatrixInverse * vec4 (gl_LightSource[0].position);
	vec3 light = normalize ((light_pos - gl_Vertex).xyz);

	// TBN: convert tangent space to object space
	// TBN_INV: convert object space to tangent space 
	mat3 TBN_INV = mat3 (normalize(Tangent), normalize(Bitangent), normalize (gl_Normal));
	eyeVec = TBN_INV * view;
	lightVec = TBN_INV * light;
}