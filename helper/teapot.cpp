#include "teapot.h"
#include "gl/gluit.h"
#include <iostream>

void Teapot::show()
{	
	if (status == 1) {   // draw un-grabbed teapot
		glPushAttrib (GL_ENABLE_BIT);
		glDisable (GL_TEXTURE_2D);
		glEnable (GL_LIGHTING);
		float yellow[] = {1,1,0,1};
		glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
		glPushMatrix();
		glTranslated (pos[0],pos[1],pos[2]);
		glutSolidTeapot (0.2);
		glPopMatrix();
		glPopAttrib();
	} else if (status == 0) {  // draw teapot after grab --> score
		glPushAttrib (GL_ENABLE_BIT);
		glDisable (GL_LIGHTING);
		setfont ("times roman", 24);
		glDisable (GL_TEXTURE_2D);
		Drawstr (pos[0], pos[1], pos[2], "10");
		glPopAttrib();
		
		// set up a private variable "count" in teapot.h
		// after the status becomes 0, the count starts to count down
		// when the count reaches zero, the status becomes -1 (invisible && ungrabbable)
	}
}
