#include "gluit.h"
#include "gl/glew.h"
#include "svl/svl.h"

class Teapot 
{
public:	
	Teapot();
	Teapot(Vec3 position);
	Teapot(Vec3 position, double[] color);


private:
	Vec3 pos;
	double[] color;
};