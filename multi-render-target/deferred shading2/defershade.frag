uniform sampler2D img_diffuse;
uniform sampler2D img_normal;
uniform sampler2D img_depth;  // NDC
uniform sampler2DShadow shadowMap;

void main()
{
	if (texture2D (img_depth, gl_TexCoord[0].st).z > 0.996) 
		gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0);
	else {
		vec3 ndcp = texture2D (img_depth, gl_TexCoord[0].st).xyz;
		ndcp = (ndcp - vec3(0.5))*2.0;
		vec4 eyepos = gl_ProjectionMatrixInverse * vec4 (ndcp,1.0);
		vec4 LSCoord = gl_TextureMatrix[3] * eyepos;
#if 1	
	float kTransparency = 0.5;    
    float rValue = shadow2DProj (shadowMap, LSCoord).r + kTransparency;
    rValue = clamp (rValue, 0.0, 1.0);
#endif
		vec3 normal = texture2D (img_normal, gl_TexCoord[0].st).xyz;
		//normalize (normal);
		normal = (normal - vec3(0.5,0.5,0.5))*2.0;
		float intensity = rValue*dot (normal, normalize (gl_LightSource[0].position.xyz));
		
		gl_FragColor = intensity*texture2D (img_diffuse, gl_TexCoord[0].st);
	}
}
