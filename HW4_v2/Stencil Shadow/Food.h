#include "gl/glew.h"
#include "gl/glpng.h"
#include "gl/gluit.h"
#include "svl/svl.h"

class Food 
{
public:	
	Food();
	Food(Vec3 position);
	void show();
	void shadow();

	static GLMmodel* food_model;

	Vec3 pos;
	int score;
	int status;

	static float self_rotation_angle;

private:
	int count;
};

