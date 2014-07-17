//
// I leave out the specular & ambient intentionally
// please add them in.
//
uniform sampler2DShadow shadowMap;
uniform sampler2D sceneMap; // texture for scene
varying vec4 LSCoord; // Light Source Coord.
varying vec4 diffuse;

// use glUniform1i () to set it (0 for false, true otherwise)
uniform bool textured_object;  

void main ()
{
    vec4 color = diffuse;
    if (textured_object) 
		color = color*texture2D (sceneMap, gl_TexCoord[0].st);
	
	float kTransparency = 0.5;
    
    float rValue = shadow2DProj (shadowMap, LSCoord).r + kTransparency;
    rValue = clamp (rValue, 0.0, 1.0);

    if (LSCoord.w > 0.0)  
        gl_FragColor = color*rValue;
    else
        gl_FragColor = color;
}
