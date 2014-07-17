#include "svl/svl.h"

class Teapot 
{
public:
	Vec3 pos;
	int score;
	int status;
	
	Teapot(){status = 1;};
	void show();
};

