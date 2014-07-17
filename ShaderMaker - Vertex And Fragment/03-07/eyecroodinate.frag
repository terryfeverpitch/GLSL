// simple fragment shader

// 'time' contains seconds since the program was linked.
uniform float time;

varying vec4 eyePos;

void main()
{
	vec4 color;

	if(eyePos.x > 0.0)
		color = vec4(1.0, 0.0, 0.0, 1.0);
	else 
		color = vec4(0.0, 1.0, 0.0, 1.0);

	gl_FragColor = color;
}
