#include "svl/svl.h"

class Teapot 
{
public:	
	Teapot();
	Teapot(Vec3 position);
	void show();

	Vec3 pos;
	int score;
	int status;
	
private:
	int count;
};

