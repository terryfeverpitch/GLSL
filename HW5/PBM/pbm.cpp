//
// PBM tutorial
// (Fall 2014 version)
//


#include "pbm.h"
#pragma comment (lib, "svl-vc9.lib")

//#include <assert.h>

extern void ForceField (Particle);

Particle ParticleNew (ParticleSystem ps, const Vec3& pos)
{
#define MASS 1.0
	Particle p = (Particle) malloc (sizeof (PARTICLE));
	p->x = pos; p->v = p->f = vl_0;
	p->m = MASS;
	p->anchor = 0;
	ps->particles.push_back (p);
	return p;
}

Spring SpringNew (ParticleSystem ps, const Particle p1, const Particle p2)
{
	Spring sp = (Spring) malloc (sizeof (SPRING));
	sp->p1 = p1; sp->p2 = p2;
	sp->ucl = len (p1->x - p2->x);
	ps->springs.push_back (sp);
	return sp;
}

Plane PlaneNew (ParticleSystem ps, const Vec3& point, const Vec3& normal)
{
	Plane p = (Plane) malloc (sizeof (PLANE));
	p->point = point;
	p->normal = normal;
	ps->planes.push_back (p);
	return p;
}


ParticleSystem ParticleSystemNew ()
{
	ParticleSystem p = (ParticleSystem) calloc (1, sizeof (PARTICLESYSTEM));
	return p;
}

int ParticleStateDim (ParticleSystem p)
{
    return (6 * p->particles.size());
}

void FinishParticleSystem (ParticleSystem p)
{
    int dim = ParticleStateDim (p);
	
    p->state_old.SetSize (dim); 
    p->state_new.SetSize (dim);
    p->state_dot.SetSize (dim);

	p->t = 0.0;
	
	p->mouse_loc = vl_0;
	p->pulled = -1;   // no one got pulled
}


void GetParticleState (const ParticleSystem p, double *dst)
{
	int psize = p->particles.size();
	
    for (int i = 0; i < psize; i++) {
	*(dst++) = p->particles[i]->x[0];
	*(dst++) = p->particles[i]->x[1];
	*(dst++) = p->particles[i]->x[2];
	*(dst++) = p->particles[i]->v[0];
	*(dst++) = p->particles[i]->v[1];
	*(dst++) = p->particles[i]->v[2];
    }
}

void SetParticleState (ParticleSystem p, const double *src)
{
	int psize = p->particles.size();
    for (int i = 0; i < psize; i++) {
	p->particles[i]->x[0] = *(src++);
	p->particles[i]->x[1] = *(src++);
	p->particles[i]->x[2] = *(src++);
	p->particles[i]->v[0] = *(src++);
	p->particles[i]->v[1] = *(src++);
	p->particles[i]->v[2] = *(src++);
    }
}


/////////////////
// forces ..
/////////////////


static void ApplyDampedSpring(Particle p1, Particle p2, double r)
{	
	Vec3 dx = p1->x - p2->x;
	Vec3 dv = p1->v - p2->v;
	double dx_len = len (dx);
	double scale = - (ks * (dx_len - r) + kd * dot (dv, dx) / dx_len) / dx_len;
    p1->f += scale * dx;
	p2->f -= scale * dx;
}

static void ApplyMouseSpring(const Vec3 &mouse_loc, Particle p2, double r)
{
#define mouse_ks  (ks/.5)   // looks better if the mouse spring is a bit softer ??

	Vec3 zero = vl_0;
	Vec3 dx = mouse_loc - p2->x;
	Vec3 dv = zero - p2->v;
	double dx_len = len(dx);
	
 	double scale = - (mouse_ks * (dx_len - r) + kd * dot (dv, dx) / dx_len) / dx_len;
	p2->f -= scale * dx;
}


void ComputeForces (ParticleSystem p)
{
	int psize = p->particles.size();
	// clear forces
	for (int i = 0; i < psize; i++) 
		p->particles[i]->f = vl_0;  

	// start accumulating forces...
	for (int j = 0; j < psize; j++) 
		ForceField (p->particles[j]);

	// mouse spring
    if (p->pulled >= 0) {
		// determine which particle got acted upon
		ApplyMouseSpring(p->mouse_loc, p->particles[p->pulled], 0.0);
	}

    // other springs in the system
    for (unsigned int k = 0; k < p->springs.size(); k++)
		ApplyDampedSpring(p->springs[k]->p1, p->springs[k]->p2, p->springs[k]->ucl);
    
    // contact forces ...

	// constraint forces ...
}

void ParticleDerivative(ParticleSystem p, double *dst)
{
    double m;

	ComputeForces (p);

    // Newton's 2nd law: f=ma
	for (unsigned int i = 0; i < p->particles.size(); i++){
		m = p->particles[i]->m;
		*(dst++) = p->particles[i]->v[0];  
		*(dst++) = p->particles[i]->v[1];
		*(dst++) = p->particles[i]->v[2];
		*(dst++) = p->particles[i]->f[0]/m;
		*(dst++) = p->particles[i]->f[1]/m;
		*(dst++) = p->particles[i]->f[2]/m;
    }
}
#if 0
/////////////////////////////////////////
// collision detection
///////////////////////

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
    for (int i = 0; i < p->planes.size(); i++) {
		for (int j = 0; j < p->particles.size(); j++) { 
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
#endif

void EulerStep (ParticleSystem p, double delta_t)
{
    int dim = ParticleStateDim (p);
    int spacedim;
//	double crossFraction;

	spacedim = 3;   // we handle 3D always

    GetParticleState (p, p->state_old.Ref());   
    ParticleDerivative (p, p->state_dot.Ref());
	p->state_new = p->state_old + delta_t * p->state_dot;
    // CombinedVector (1.0, p->state_old, delta_t, p->state_dot, p->state_new, dim);

	// anchor particles do not move
	for (unsigned int i = 0; i < p->particles.size(); i++) {
		if (p->particles[i]->anchor) {
			for (int ii = 0; ii < spacedim; ii++) { 
				p->state_new[2*spacedim*i+ii] = p->state_old[2*spacedim*i+ii];
				p->state_new[2*spacedim*i+ii+spacedim] = 0.0;
			}
		}
	}



}
