//#include <gl/glew.h>
#include "gl/gluit.h"
#include "gl/glpng.h"


static float lightPosition[4] = {-3.0, 3.0, 3.0, 1.0};

GLMmodel *pmodel;

void initOccluder ()
{
	pmodel = glmReadOBJ ("al.obj");
	glmUnitize (pmodel);
	glmFacetNormals (pmodel);
	glmVertexNormals (pmodel, 90.0);
	
	//teapot: nothing to init...
}

static GLuint FloorTexture;

void initFloor(char* filename)
{
	pngSetStandardOrientation (1);
	FloorTexture = pngBind(filename, PNG_BUILDMIPMAPS, PNG_SOLID, NULL, GL_REPEAT, GL_LINEAR, GL_LINEAR);
}

float angle;

void DrawOccluder()
{
	/*glPushMatrix();
	glTranslatef(0, 1, 0);
	glRotatef(angle, 0, 1, 0);
	glutSolidTeapot(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3, 1, 1);
	glRotatef(angle, 0, 1, 0);
	glutSolidTeapot(1.0);
	glPopMatrix();*/
	glPushAttrib (GL_ENABLE_BIT);
	glDisable (GL_TEXTURE_2D);
	glPushMatrix();
		glRotatef (angle, 0,1,0);
		glTranslatef (2,0,0);
		glRotatef (180, 0,1,0);
		glTranslatef(0,1,0);
		glmDraw (pmodel, GLM_MATERIAL|GLM_SMOOTH); 
	glPopMatrix();
	glPopAttrib();

    // glUniform1i (texedobjloc, 1);  // for textured objects
    // glUniform1i (texedobjloc, 0); for un-textured objects: 	
}

void DrawFloor(float fCenterX, float fCenterY, float fCenterZ)
{
	/*glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3i(-5, -1, 5);
	glVertex3i(5, -1, 5);
	glVertex3i(5, -1, -5);
	glVertex3i(-5, -1, -5);
	glEnd();*/

	float FloorColor[] = {1,1,1,0.6};
	glMaterialfv(GL_FRONT, GL_SPECULAR, FloorColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, FloorColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 200.0);

	// set up blending so we can see the reflected cube through the
	// surface, and thus create the illusion of reflection

	glBindTexture(GL_TEXTURE_2D, FloorTexture);

	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0);

		glTexCoord2f (0,0); glVertex3f (fCenterX-5, fCenterY,fCenterZ+5);
		glTexCoord2f (10,0); glVertex3f (fCenterX+5,fCenterY,fCenterZ+5);
		glTexCoord2f (10,10); glVertex3f (fCenterX+5,fCenterY,fCenterZ-5);
		glTexCoord2f (0,10); glVertex3f (fCenterX-5,fCenterY,fCenterZ-5);

	glEnd();

}
bool turn = false;
void scene_timer ()
{
	if(turn)
		angle += .5;
}
void scene_keys (unsigned char key)
{
    switch (key) {
	case 'x':
		lightPosition[0] -= 0.1;
		break;
	case 'X':
		lightPosition[0] += 0.1;
		break;
	case ' ':
		turn ^= 1;
		break;
    }
}

void get_scene_light_pos (float* pos)
{
    for (int i = 0; i < 4; i++)
	pos[i] = lightPosition[i];
}
