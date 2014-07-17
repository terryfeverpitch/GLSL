// very simple fragment shaderke

void main()
{	
	//Yeah, yeah, yeah ... we just color the pixel
	// this example is showing off geometry shaders, not fragments! 
	//Shade to blue!
	gl_FragColor = gl_Color; //vec4(0.0,0.0,1.0,1.0);	
}
