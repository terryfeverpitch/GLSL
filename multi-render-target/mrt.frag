void main ()
{
	gl_FragData[0] = vec4 (gl_Color.r, 0., 0., 1.0);
	gl_FragData[1] = vec4 (0., gl_Color.g, 0., 1.0);
}
