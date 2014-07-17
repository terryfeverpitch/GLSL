#include <stdio.h>
#include <string.h>
#include "svl/svl.h"

// 2014 version
#include <vector>
using namespace std;

#define ks 12.0
#define kd 0.7
//#define h_step 0.05 //for Euler Method
#define	epsilon 0.9	/* coeff. of restitution */


/*
** Data structures as suggested in class
*/

typedef struct {
  double m;
  Vec3 x, v, f;
  int anchor; // 1 if anchored (not allowed to move) 
} PARTICLE, *Particle;

typedef struct {
  Particle p1, p2;
  double ucl; /* Uncompressed length */
} SPRING, *Spring;
 
typedef struct{
  Vec3 point;
  Vec3 normal;  // unit normal!
} PLANE, *Plane;

typedef struct {
  vector<Particle> particles;	
  vector<Spring> springs;
  vector<Plane> planes;

  // Dynamic system data
  double t;		/* time stamp */
  Vec state_new, state_old, state_dot; 

  // GUI plug
  Vec3 mouse_loc;   // store current mouse (world) coord if pulled by mouse spring 
  int pulled;         // the particle got pulled by mouse
                      // (-1) if not pulled
} PARTICLESYSTEM, *ParticleSystem;


// function declarations
extern Particle ParticleNew (ParticleSystem,const Vec3&);
extern Spring SpringNew (ParticleSystem, const Particle, const Particle);
extern Plane PlaneNew (ParticleSystem, const Vec3&, const Vec3&);

extern ParticleSystem ParticleSystemNew ();
extern void FinishParticleSystem (ParticleSystem);

extern int ParticleStateDim (ParticleSystem);
extern void SetParticleState (ParticleSystem, const double *);
extern void GetParticleState (ParticleSystem, double *); 
extern void ComputeForces (ParticleSystem);
extern void ParticleDerivative (ParticleSystem, double*);
extern double CollisionDetection (ParticleSystem, double*, double*, int*, int*);
extern void EulerStep (ParticleSystem, double);

extern void SetGravity (float);