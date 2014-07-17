//
// from http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
//
struct rgb {
    float r;       // percent
    float g;       // percent
    float b;       // percent
};

struct hsv {
    float h;       // angle in degrees
    float s;       // percent
    float v;       // percent
};

struct hsv rgb2hsv(struct rgb incolor)
{
    struct hsv         outcolor;
    float      min, max, delta;

    min = incolor.r < incolor.g ? incolor.r : incolor.g;
    min = min  < incolor.b ? min  : incolor.b;

    max = incolor.r > incolor.g ? incolor.r : incolor.g;
    max = max  > incolor.b ? max  : incolor.b;

    outcolor.v = max;                                // v
    delta = max - min;
    if( max > 0.0 ) {
        outcolor.s = (delta / max);                  // s
    } else {
        // r = g = b = 0                        // s = 0, v is undefined
        outcolor.s = 0.0;
        outcolor.h = 0.0;//NAN;                            // its now undefined
        return outcolor;
    }
    if( incolor.r >= max )                           // > is bogus, just keeps compilor happy
        outcolor.h = ( incolor.g - incolor.b ) / delta;        // between yellow & magenta
    else
    if( incolor.g >= max )
        outcolor.h = 2.0 + ( incolor.b - incolor.r ) / delta;  // between cyan & yellow
    else
        outcolor.h = 4.0 + ( incolor.r - incolor.g ) / delta;  // between magenta & cyan

    outcolor.h *= 60.0;                              // degrees

    if( outcolor.h < 0.0 )
        outcolor.h += 360.0;

    return outcolor;
}


struct rgb hsv2rgb(struct hsv incolor)
{
    float      hh, p, q, t, ff;
    int        i;
    struct rgb         outcolor;

    if(incolor.s <= 0.0) {       // < is bogus, just shuts up warnings
        outcolor.r = incolor.v;
        outcolor.g = incolor.v;
        outcolor.b = incolor.v;
        return outcolor;
    }
    hh = incolor.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = int(floor (hh));//(long)hh;  // ? float to int
    ff = fract (hh);//hh - i;
    p = incolor.v * (1.0 - incolor.s);
    q = incolor.v * (1.0 - (incolor.s * ff));
    t = incolor.v * (1.0 - (incolor.s * (1.0 - ff)));

    switch(i) {
    case 0:
        outcolor.r = incolor.v;
        outcolor.g = t;
        outcolor.b = p;
        break;
    case 1:
        outcolor.r = q;
        outcolor.g = incolor.v;
        outcolor.b = p;
        break;
    case 2:
        outcolor.r = p;
        outcolor.g = incolor.v;
        outcolor.b = t;
        break;

    case 3:
        outcolor.r = p;
        outcolor.g = q;
        outcolor.b = incolor.v;
        break;
    case 4:
        outcolor.r = t;
        outcolor.g = p;
        outcolor.b = incolor.v;
        break;
    case 5:
    default:
        outcolor.r = incolor.v;
        outcolor.g = p;
        outcolor.b = q;
        break;
    }
    return outcolor;     
}

varying vec4 eyepos;
varying vec3 eyenorm;

void main()
{
	float ndotl = dot (normalize(eyenorm),
   		normalize (gl_LightSource[0].position.xyz));

	vec3 c_rgb = gl_FrontLightProduct[0].diffuse.rgb;

	struct rgb rr;
	rr.r = c_rgb.r; rr.g = c_rgb.g; rr.b = c_rgb.b;

	struct hsv c_hsv = rgb2hsv (rr);
	
	if (ndotl > 0.8) {
		c_hsv.v *= 1.0; 
	} else if (ndotl  > 0.5) {
		c_hsv.v *= 0.6;
	} else {
		c_hsv.v *= 0.3;
	}
	// convert back to RGB
	rr = hsv2rgb (c_hsv);
	vec3 cc = ndotl* vec3 (rr.r, rr.g, rr.b);
 
	gl_FragColor = vec4 (cc, 1.0);
}
