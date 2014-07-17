// Phone fragment shader

varying vec3 ecnormal;
varying vec3 ecposition;

void main()
{
	vec3 color = vec3(.2,.2,.2);
	ecnormal = normalize (ecnormal);

	float ndotl = max (0.0, dot(ecnormal, 
		normalize (gl_LightSource[0].position.xyz - ecposition)));

	color += ndotl * vec3 (.7,.7,.7);

	vec3 halfvector = normalize(normalize(gl_LightSource[0].position.xyz-ecposition) 
			- normalize(ecposition));
	if (ndotl > 0.0) {
		float ndoth = dot (ecnormal, halfvector);
		color += pow (ndoth, 64.) * vec3 (1,0,0);
	}

	gl_FragColor = vec4 (color, 1.0);
}
