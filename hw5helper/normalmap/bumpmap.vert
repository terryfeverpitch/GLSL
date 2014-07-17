// bump map
attribute vec3 Tangent;
attribute vec3 Bitangent;

varying vec3 lightDir;  // lvec in tangent space

void main()
{
	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	// transform input TBN to eye space
	vec3 t = normalize (gl_NormalMatrix * Tangent);
	vec3 b = normalize (gl_NormalMatrix * Bitangent);
	vec3 n = normalize (gl_NormalMatrix * gl_Normal);
	vec4 eposition = gl_ModelViewMatrix * gl_Vertex;

	vec3 lvec = vec3 (gl_LightSource[0].position - eposition);
	vec3 v;
	v.x = dot (lvec, t);
	v.y = dot (lvec, b);
	v.z = dot (lvec, n);
	lightDir = normalize (v);
}
