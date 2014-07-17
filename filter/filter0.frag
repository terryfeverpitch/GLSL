#version 120   // must add for array constructors
#define KERNEL_SIZE 9

// Gaussian kernel
// 1 2 1
// 2 4 2
// 1 2 1
// Laplacian kernel
// 0 -1 0
// -1 4 -1
// 0 -1 0

uniform sampler2D colorMap;
uniform float width;   // size of image
uniform float height;

float step_w = 1.0/width;
float step_h = 1.0/height;

float kernel[KERNEL_SIZE];
vec2 offset[KERNEL_SIZE];

void init_kernel()
{
#if 1
	kernel = float[] (
				0.0, -1.0, 0.0,
				-1.0, 4.0, -1.0,
				0.0, -1.0, 0.0);
#else
	kernel = float[] (
				1.0/16.0, 2.0/16.0, 1.0/16.0,
				2.0/16.0, 4.0/16.0, 2.0/16.0,
				1.0/16.0, 2.0/16.0, 1.0/16.0);
#endif
	offset = vec2[] ( vec2(-step_w, -step_h), vec2(0.0, -step_h), vec2(step_w, -step_h), 
				vec2(-step_w, 0.0), vec2(0.0, 0.0), vec2(step_w, 0.0), 
				vec2(-step_w, step_h), vec2(0.0, step_h), vec2(step_w, step_h) );
}

void main(void)
{
   int i = 0;
   vec4 sum = vec4(0.0);
   
   init_kernel();
   
   if(gl_TexCoord[0].s < 0.49) {
        for( i=0; i<KERNEL_SIZE; i++ )  {
	    vec4 tmp = texture2D(colorMap, gl_TexCoord[0].st + offset[i]);
	    sum += tmp * kernel[i];
	}
   } else if( gl_TexCoord[0].s > 0.51 ) {
	sum = texture2D(colorMap, gl_TexCoord[0].st);
   } else {
	sum = vec4(1.0,0.0,0.0,1.0);
   }

   gl_FragColor = sum;
}

