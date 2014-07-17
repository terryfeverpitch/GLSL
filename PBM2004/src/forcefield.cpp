#include "pbm.h"

static float  G=2.;

void SetGravity (float g)
{
    G = g;
}

void ForceField (Particle par)
{
	// additive force fields

	// gravity
	par->f[1] -= par->m * G;

	// other position related fields
	// viscous drag (help stablizing the system)
	par->f -= par->v * 0.4;
}