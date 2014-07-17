// simple fragment shader

varying float ndotl_out;

void main()
{
	vec3 color = ndotl_out * vec3 (1.0, 1.0, 1.0);
	gl_FragColor = vec4 (color, 1.0);
}
