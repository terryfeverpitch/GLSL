// simple fragment shader
uniform sampler2D fronttex;
uniform sampler2D backtex;

void main()
{
	if (gl_FrontFacing) 
		gl_FragColor = texture2D (fronttex, gl_TexCoord[0].st);
	else {
		vec2 st = gl_TexCoord[0].st;
		st.s = 1.0 - st.s;
		gl_FragColor = texture2D (backtex, st);
	}

}
