//
// Emboss Shader
//
uniform float pixelWidth;
uniform float pixelHeight;

varying vec2 v_vTexcoord;
varying vec4 v_vColour;

uniform sampler2D gm_BaseTexture;

void main()
{
    vec2 onePixel = vec2(1.0 / pixelWidth, 1.0 / pixelHeight);
    vec2 texCoord = v_vTexcoord;
    vec4 color1 = vec4(0, 0, 0, 1);
    
    color1.rgb = vec3(0.5);
    color1 -= texture2D(gm_BaseTexture, texCoord - onePixel) * 5.0;
    color1 += texture2D(gm_BaseTexture, texCoord + onePixel) * 5.0;
    
    color1.rgb = vec3((color1.r + color1.g + color1.b) / 3.0);
    gl_FragColor = vec4(color1.rgb, texture2D(gm_BaseTexture,v_vTexcoord).a);
} 