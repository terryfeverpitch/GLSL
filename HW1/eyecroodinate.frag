// simple fragment shader

// 'time' contains seconds since the program was linked.
uniform float time;
uniform vec4 color_positive;
uniform vec4 color_negative;
uniform int type;
varying vec4 eyePos;

void main()
{
	vec4 color;

	if(eyePos[type] > 0.0)
		color = color_positive;
	else 
		color = color_negative;
		
	gl_FragColor = color;
}
