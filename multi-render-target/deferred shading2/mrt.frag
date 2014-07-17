// MRT [0]: diffuse
// MRT [1]: normal
// MRT [2]: NDC[depth]

varying vec3 eyenormal;
varying vec4 eyepos;

void main ()
{
	// normal treatment ...
	vec3 frag_n = normalize (eyenormal); // [-1,1]^3
	frag_n = (frag_n + vec3 (1.))/2.; // [0,1]^3	

	// depth treatment ...
	vec4 clipos = gl_ProjectionMatrix * eyepos;
	//float depth = clipos.z/clipos.w;
	//depth = (depth + 1.0)/2.0;
	vec3 ndc = clipos.xyz/clipos.w;  // [-1,1]^3
	ndc = (ndc + vec3(1.0))/2.0;     // [0,1]^3
	
	gl_FragData[0] = gl_FrontMaterial.diffuse; 
	gl_FragData[1] = vec4 (frag_n, 1.0);
//	gl_FragData[2] = vec4 (vec3(depth),1.0);	
	gl_FragData[2] = vec4 (ndc, 1.0);
}
