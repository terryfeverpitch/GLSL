#include "pbm.h"

static void SetXYBoundingBox (ParticleSystem ps, const Vec3& min, const Vec3& max)
// motion in XY plane; four planes got set
{
	double min_x, min_y, max_x, max_y;

	min_x = min[0], min_y = min[1];
	max_x = max[0], max_y = max[1];
	
	PlaneNew (ps, Vec3 (min_x, max_y, 0.0), Vec3 (0.0, -1.0, 0.0));  // upper
    PlaneNew (ps, Vec3 (max_x, max_y, 0.0), Vec3 (-1.0, 0.0, 0.0));  // right
    PlaneNew (ps, Vec3 (max_x, min_y, 0.0), Vec3(0.0, 1.0, 0.0));    // bottom
    PlaneNew (ps, Vec3 (min_x, min_y, 0.0), Vec3(1.0, 0.0, 0.0));    // left 
}

#define SET_ANCHOR(par) {\
	(par)->anchor = 1; \
}

#define UNSET_ANCHOR(par) {\
	(par)->anchor = 0; \
}


ParticleSystem InitModel()
{
	ParticleSystem ps = ParticleSystemNew ();

	Particle p[2];   // pointer array, for reference
	p[0] = ParticleNew (ps, Vec3(-1,-1.1,0));
	p[1] = ParticleNew (ps, Vec3(1,-1,0));

	SpringNew (ps, p[0], p[1]);

//	SET_ANCHOR (p[0]);

	SetXYBoundingBox (ps, Vec3(-5,-5,0), Vec3(5,5,0));
	
	FinishParticleSystem (ps);  // required!

	return ps;	
}


ParticleSystem InitModel0 () 
{
	ParticleSystem ps = ParticleSystemNew ();

	Particle p[4];
	
	p[0] = ParticleNew (ps, Vec3(-1,-1,0));
	p[1] = ParticleNew (ps, Vec3(1,-1,0));
	p[2] = ParticleNew (ps, Vec3(0,-2,0));
	p[3] = ParticleNew (ps, Vec3(1,1,0));

	SpringNew (ps, p[3],p[1]);
	SpringNew (ps, p[1],p[2]);
	SpringNew (ps, p[2],p[0]);

	SET_ANCHOR (p[3]);

	SetXYBoundingBox (ps, Vec3(-5,-5,0), Vec3(5,5,0));
	FinishParticleSystem (ps);
	return ps;
	
}