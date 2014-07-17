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

strcut hsv rgb2hsv(struct rgb incolor)
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
        outcolor.h = NAN;                            // its now undefined
        return out;
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

    return out;
}


struct rgb hsv2rgb(struct hsv incolor)
{
    float      hh, p, q, t, ff;
    long        i;
    struct rgb         outcolor;

    if(incolor.s <= 0.0) {       // < is bogus, just shuts up warnings
        outcolor.r = incolor.v;
        outcolor.g = incolor.v;
        outcolor.b = incolor.v;
        return out;
    }
    hh = incolor.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
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
    return out;     
}