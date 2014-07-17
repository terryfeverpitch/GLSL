#include "pbm.h"

#include <assert.h>

static int OutOfBound (const Vec3 &x, Plane wall)
{
    // double tmp[3];
    // SubVecTo (x, wall->point, tmp);

	Vec3 tmp = x - wall->point;
    return (dot(wall->normal, tmp) < 0.0);
}

// linear interpolation between [oldpos, newpos] that hit the wall
static double GetCrossFraction (Plane wall, const Vec3 &oldpos, const Vec3 &newpos)
{
    // double t, tmp1[3], tmp2[3];
    // SubVecTo (oldpos, wall->point, tmp1);
    // SubVecTo (oldpos, newpos, tmp2);
	
	Vec3 tmp1 = oldpos - wall->point;
	Vec3 tmp2 = oldpos - newpos;
	double t = dot (tmp1, wall->normal) / dot (tmp2, wall->normal);
    // did not take into account of zero denominator: <(p0-p1), n> = 0
    return t;
}


// double sided wall?!
double CollisionDetection (  // returns crossFraction...
   ParticleSystem p, 
   const Vec &state_old,   // state (t)
   const Vec &state_new,   // state (t+dt)
   int &wall, int &par  // the (first) colliding particle and corresponding wall
)
{
    Vec3 pos, oldpos;
    double crossFraction, t; // fraction: [0,1]
	
    // find the first collision (if any)
    crossFraction = 1.0;	// any constant > 1.0
    wall = par = -1;
    for (unsigned int i = 0; i < p->planes.size(); i++) {
		for (unsigned int j = 0; j < p->particles.size(); j++) { 
			pos = Vec3 (state_new[6*j+0], state_new[6*j+1], state_new[6*j+2]);

			if (OutOfBound (pos, p->planes[i])) {
				oldpos = Vec3 (state_old[6*j+0], state_old[6*j+1], state_old[6*j+2]);
				t = GetCrossFraction (p->planes[i], oldpos, pos);
				if (t > 0 && t < crossFraction) {
					crossFraction = t;
					// record: i, j
					wall = i, par = j;
				}
			}
		}
	}

	// if all in bound, return 1.0 (acknowledge the newpos)
	// and *wall is -1
    return crossFraction;
}

void CollisionDriver (ParticleSystem p, double delta_t)
{
	double crossFraction;
	int plane;
	int par;

	// collision detection
	crossFraction = CollisionDetection (p, p->state_old, p->state_new, plane, par);

	assert (crossFraction >= 0 && crossFraction <= 1);

	// collision response
	double t_inc, imp;
    if (plane != -1) {  // collision occurs
          t_inc = delta_t*crossFraction; // move the detected fraction
          p->state_new = p->state_old + t_inc * p->state_dot;
          SetParticleState (p, p->state_new.Ref());

          // apply impulse to collided particles	
		  imp = -(1 + epsilon) * dot (p->planes[plane]->normal, p->particles[par]->v) ;
		  p->particles[par]->v = p->particles[par]->v + imp * p->planes[plane]->normal;
		  
          p->t += t_inc;
    } else {
          SetParticleState(p, p->state_new.Ref());
          p->t += delta_t;
    }


}
