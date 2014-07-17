varying vec4 ecposition;
varying vec3 normal;
uniform sampler2D sphmap;

vec2 SphereMap (const in vec3 U, const in vec3 N)
{
	vec3 R;
	R = reflect (U, N);
	R.z += 1.0;
	R = normalize (R);
	return R.xy * 0.5 + 0.5;
}

void main()
{
	vec2 tc = SphereMap (normalize (ecposition.xyz), 
                         normalize (normal));
	
	gl_FragColor = texture2D (sphmap, tc); 
//	gl_FragColor = vec4 (1.0,0.0, 0.0, 1.0);
//	gl_FragColor = vec4 (tc.s, tc.t, 0.0,1.0);
}

