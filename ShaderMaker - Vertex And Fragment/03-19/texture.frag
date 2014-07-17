uniform sampler2D mytex1, mytex2;
uniform float time;
uniform float weight;

void main()
{
	vec2 st = gl_TexCoord[0].st;
	st.x += time / 2.0;

	vec4 tex1Color = texture2D(mytex1, gl_TexCoord[0].st);
	vec4 tex2Color = texture2D(mytex2, gl_TexCoord[0].st);
	gl_FragColor = (weight * tex1Color) + (1.0- weight) * tex2Color;
}
