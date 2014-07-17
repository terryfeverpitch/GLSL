// from typhoonlab's GSLS tutorial Chap.4

uniform sampler2D colormap;
uniform sampler2D heightmap;
uniform sampler2D normalmap;

uniform vec2 scaleBias;

varying vec3 eyeVec;  // (interpolated) view vector in TS
varying vec3 lightVec;  // (interpolated) light vector in TS
void main()
{
	float height = texture2D (heightmap, gl_TexCoord[0].st).r;

	float h = height * scaleBias.r - scaleBias.g;

	vec3 eye = normalize (eyeVec);
	vec2 newCoords = gl_TexCoord[0].st + eye.xy * h;

	vec3 rgb = texture2D (colormap, newCoords).rgb;

    vec3 normal = 2.0*texture2D (normalmap, newCoords).rgb - vec3(1.,1.,1.);
	float diffuse = max (0.0, dot (normalize(normal), normalize (lightVec)));

	gl_FragColor = vec4 (diffuse*rgb, 1.0);
}
