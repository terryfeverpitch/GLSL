attribute float temperature;
varying float norm_temp;

uniform float cool_temp;
uniform float hot_temp;

void main ()
{
    norm_temp = (temperature - cool_temp)
	              /(hot_temp - cool_temp);
    
    gl_Position = ftransform();
}
