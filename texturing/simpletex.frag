uniform sampler2D tex1;
uniform sampler2D tex2;

void main()
{
	gl_FragColor = texture2D (tex1, gl_TexCoord[0].st) + 
				   texture2D (tex2, gl_TexCoord[0].st);
}