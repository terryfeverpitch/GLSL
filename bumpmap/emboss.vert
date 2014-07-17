//
// Simple passthrough vertex shader
//

varying vec2 v_vTexcoord;
varying vec4 v_vColour;

void main()
{
    vec4 object_space_pos = vec4(gl_Vertex.xyz, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * object_space_pos;
    
    v_vColour = gl_Color;
    v_vTexcoord = gl_MultiTexCoord0.st;
}