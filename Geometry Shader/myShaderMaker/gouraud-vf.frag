// simple fragment shader

varying float ndotl;

void main()
{
	vec3 color = ndotl * vec3 (1.0, 1.0, 1.0);
	gl_FragColor = vec4 (color, 1.0);
}
