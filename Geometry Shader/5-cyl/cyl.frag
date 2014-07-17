//CIRL GPU Fragment Program: Derek Anderson and Robert Luke
// very simple fragment shader

varying float lightIntensity;

void main()
{	
#if 0
	float ndotl = max(0., dot(normalize(ecnormal_out), vec3(0,0,1)));
	ndotl = 0.4;
	gl_FragColor = vec4(ndotl * vec3(0.0,0.0,1.0),1.0);	
#else
	gl_FragColor = vec4 (lightIntensity*vec3(1,1,1), 1.0);
#endif
}
