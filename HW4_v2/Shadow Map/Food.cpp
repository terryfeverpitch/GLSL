#include "Food.h"
#include "gl/gluit.h"
#include <iostream>

float Food::self_rotation_angle = 0.0;
GLMmodel* Food::food_model = 0;

Food::Food() 
{
	status = 1; 
	count  = 120;
	score  = 10;
	pos = Vec3(0, 0, 0);
}

Food::Food(Vec3 position) 
{
	status = 1; 
	count  = 120;
	score  = 10;
	pos    = position;
}

void Food::show()
{	
	if(status == 1) {   // draw un-grabbed
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glPushMatrix();
		glTranslated(pos[0], pos[1], pos[2]);
		glRotatef(self_rotation_angle, 0,1,0);
		glmDraw(food_model, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();
		glPopAttrib();
	} 
	else if(status == 0) {  // draw after grab --> score
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		setfont("defalult", 24);
		count -= 1;
		glColor3f(1, 1, 1);
		Drawstr(pos[0], pos[1], pos[2], "10");
		glPopAttrib();
		
		if(count <= 0)
			status = -1;
	}
	// status = -1, Do nothing.
}

void Food::shadow()
{	
	if(status == 1) {
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glPushMatrix();
		glTranslated(pos[0],pos[1],pos[2]);
		glRotatef(self_rotation_angle, 0,1,0);
		glmDraw(food_model, GLM_SMOOTH);
		glPopMatrix();
		glPopAttrib();
	} 
	else if(status == 0) {	
		if(count <= 0)
			status = -1;
	}
	// status = -1, Do nothing.
}
