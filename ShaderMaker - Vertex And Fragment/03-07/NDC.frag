// simple fragment shader

// 'time' contains seconds since the program was linked.
uniform float time;

varying vec4 clip_pos;

void main()
{
	vec3 p = vec3(clip_pos.x / clip_pos.w,
				  clip_pos.y / clip_pos.w,
				  clip_pos.z / clip_pos.w);
	float dst = 1.0 - length(p.xy);//sqrt(p.x * p.x + p.y * p.y);//abs( + p.y * p.y);
	vec4 color = vec4(dst, dst, dst, 1.0);
	gl_FragColor = color;
}
