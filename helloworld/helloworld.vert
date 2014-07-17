// helloworld.vert


void main ()
{
    vec4 mypos = gl_Vertex;

    mypos.y = 0.5* sin (5.0*mypos.x); 
    
    gl_Position = gl_ModelViewProjectionMatrix * mypos;
}
