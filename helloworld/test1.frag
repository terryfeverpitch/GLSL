// simple fragment shader
varying vec4 myPosition;
// 'time' contains seconds since the program was linked.
uniform float time;
vec4 myColor;

void main()
{
	if(myPosition.x > 0.0)
		myColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		myColor = vec4(0.0, 1.0, 0.0, 0.0);

	gl_FragColor = myColor;
}
