uniform sampler2D wallmap;
uniform sampler2D alphamap;
uniform bool dighole;

void main()
{
	vec4 mcolor = texture2D (wallmap, gl_TexCoord[0].st);
	float alpha = texture2D (alphamap, gl_TexCoord[0].st).a;

	if (dighole && alpha < 1.0)
		discard;
	else
		gl_FragColor = mcolor;
}
