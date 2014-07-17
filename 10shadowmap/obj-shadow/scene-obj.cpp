#include <stdio.h>
#include <gl/glew.h>
#include "gl/gluit.h"
#include "gl/glpng.h"

static int floorTid;
static int texedobjloc;
GLMmodel *pmodel, *almodel;

void initScene(int program)
{

	pngSetStandardOrientation(1);
	glActiveTexture (GL_TEXTURE0);  // TU #0
	floorTid = pngBind("marble.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_NEAREST, GL_NEAREST);

	pmodel = glmReadOBJ ("penguin.obj");
	glmUnitize (pmodel);
	glmFacetNormals(pmodel);
	glmVertexNormals(pmodel,90.0);
	
	almodel = glmReadOBJ ("al.obj");
	glmUnitize (almodel);
	glmFacetNormals(almodel);
	glmVertexNormals(almodel,90.0);

	int sceneMaploc = glGetUniformLocation (program, "sceneMap");
	glUniform1i (sceneMaploc, 0);  // scene use TU #0

	texedobjloc = glGetUniformLocation (program, "textured_object");  // texture_object == 1 for textured object
}

//static float angle = 0.0;
//static int rotate = 1;
void scene_timer()
{
//	if (rotate) angle += 5.0;
}

static float lightPosition[4] = {-1,3,3,1};

void scene_keys (unsigned char key)
{
	switch (key) {
	case 'X':
		lightPosition[0] += .5;
		break;
	case 'x':
		lightPosition[0] -= .5;
		break;
	}
}

void get_scene_light_pos (float *pos)
{
	for (int i = 0; i < 4; i++)
		pos[i] = lightPosition[i];
}

static void DrawFloor(float fCenterX, float fCenterY, float fCenterZ)
{
	float FloorColor[] = {1.,1.,1.,1.};
	glMaterialfv(GL_FRONT, GL_SPECULAR, FloorColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, FloorColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 200.0);

	// set up blending so we can see the reflected cube through the
	// surface, and thus create the illusion of reflection
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTid);

	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0);

		// to have lighting effects show up at all, we need to draw the
		// surface as a lot of quads, not just one
		float x = fCenterX-5.0f, z = fCenterZ-7.0f;

		for (GLint i = 0; i < 10; i++, x += 1.0)
		{
			for (GLint j = 0; j < 14; j++, z += 1.0)
			{
			  // draw the plane slightly offset so the shadow shows up
			  glTexCoord2f(0.0, 0.0);
			  glVertex3f(x,			fCenterY, z);
			  glTexCoord2f(1.0, 0.0);
			  glVertex3f(x + 1.0f,	fCenterY, z);
			  glTexCoord2f(1.0, 1.0);
			  glVertex3f(x + 1.0f,	fCenterY, z + 1.0f);
			  glTexCoord2f(0.0, 1.0);
			  glVertex3f(x,			fCenterY, z + 1.0f);
			}

			z = fCenterZ-7.0f;
		}

	glEnd();
}


void DrawScene()
{
	glUniform1i (texedobjloc, 1);
	DrawFloor (0,0,0);
	
	glUniform1i (texedobjloc, 0);
	glPushMatrix();
	glTranslatef (0,.85,0);
	glmDraw (almodel, GLM_SMOOTH|GLM_MATERIAL);
	glPopMatrix();

	glUniform1i (texedobjloc, 1);
	glPushMatrix();
	glTranslatef (1,.85,1);
	glmDraw (pmodel, GLM_SMOOTH|GLM_MATERIAL|GLM_TEXTURE);
	glPopMatrix();

	glUniform1i (texedobjloc, 1);
	glPushMatrix();
	glTranslatef (-0.8,.85,1.6);
	glmDraw (pmodel, GLM_SMOOTH|GLM_MATERIAL|GLM_TEXTURE);
	glPopMatrix();

}
