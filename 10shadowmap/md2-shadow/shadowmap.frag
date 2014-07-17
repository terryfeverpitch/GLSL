//
// I leave out the specular & ambient intentionally
// please add them in.
//
uniform sampler2DShadow shadowMap;
uniform sampler2D sceneMap;
varying vec4 projCoord;
varying vec4 diffuse;

uniform int texed_obj;

void main ()
{
    vec4 color = vec4(1.,1.,1.,1.);//diffuse; // only diffuse
//    if (texed_obj == 1) 
		color = color*texture2D (sceneMap, gl_TexCoord[0].st);
	
	float kTransparency = 0.5;
    
    float rValue = shadow2DProj (shadowMap, projCoord).r + kTransparency;
    rValue = clamp (rValue, 0.0, 1.0);

    // coord in "modified" light space: [0,1]^3
    vec3 coordPos = projCoord.xyz/projCoord.w;
	
	//gl_FragColor = vec4 (rValue,rValue,rValue,1.0);
#if 1	
    if (projCoord.w > 0.0) //&& 
	//    coordPos.x >= 0.0 && coordPos.x <= 1.0 &&  
    //    coordPos.y >= 0.0 && coordPos.y <= 1.0)
        gl_FragColor = color*rValue;
    else
        gl_FragColor = color;
#endif
		}
