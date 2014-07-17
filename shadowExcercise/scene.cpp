#include <stdio.h>
#include <gl/glew.h>
#include <glut.h>
#include "gl/glpng.h"

static int brickTid;
static int texedobjloc;

void initScene(int program)
{
	glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable (GL_COLOR_MATERIAL);

	pngSetStandardOrientation(1);
	glActiveTexture (GL_TEXTURE0);  // TU #0
	brickTid = pngBind("brick.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_NEAREST, GL_NEAREST);

	int sceneMaploc = glGetUniformLocation (program, "sceneMap");
	glUniform1i (sceneMaploc, 0);  // scene use TU #0

	texedobjloc = glGetUniformLocation (program, "textured_object");  // texture_object == 1 for textured object
}

static float angle = 0.0;
static int rotate = 1;
void scene_timer()
{
	if (rotate) angle += 5.0;
}

static float lightPosition[4] = {-1,3,3,1};
void get_scene_light_pos (float* pos)
{
	for (int i =0; i < 4; i++)
		pos[i] = lightPosition[i];
}

void scene_keys (unsigned char key)
{
	switch (key) {
	case 'r':
		rotate ^= 1;
		break;
	case 'x': 
		lightPosition[0] -= 0.1;
		break;
	case 'X': 
		lightPosition[0] += 0.1;
		break;
	}
}

void DrawScene()
{
	//Display lists for objects
	static GLuint spheresList=0, torusList=0, baseList=0;

	//Create spheres list if necessary
	if(!spheresList)
	{
		spheresList=glGenLists(1);
		glNewList(spheresList, GL_COMPILE);
		{
			glColor3f(0.0f, 1.0f, 0.0f);
			glPushMatrix();
			glTranslatef(0.45f, 1.0f, 0.45f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(-0.9f, 0.0f, 0.0f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(0.0f, 0.0f,-0.9f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(0.9f, 0.0f, 0.0f);
			glutSolidSphere(0.2, 24, 24);

			glPopMatrix();
		}
		glEndList();
	}

	//Create torus if necessary
	if(!torusList)
	{
		torusList=glGenLists(1);
		glNewList(torusList, GL_COMPILE);
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glPushMatrix();

			glTranslatef(0.0f, 0.5f, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glutSolidTorus(0.2, 0.5, 24, 48);

			glPopMatrix();
		}
		glEndList();
	}

	//Create base if necessary
	if(!baseList)
	{
		baseList=glGenLists(1);
		glNewList(baseList, GL_COMPILE);
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin (GL_QUADS);
			glNormal3f (0,1,0);
			glMultiTexCoord2i (GL_TEXTURE0, 0,0); glVertex3i (3,0,3);
			glMultiTexCoord2i (GL_TEXTURE0, 1,0); glVertex3i (3,0,-3);
			glMultiTexCoord2i (GL_TEXTURE0, 1,1); glVertex3i (-3,0,-3);
			glMultiTexCoord2i (GL_TEXTURE0, 0,1); glVertex3i (-3,0,3);
			glEnd();
		}
		glEndList();
	}

	////////////////////////////////////////////////////////////
	// Scene Rendering 
	glUniform1i (texedobjloc, 1);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, brickTid);
	glCallList(baseList);

	glUniform1i (texedobjloc, 0);
	glCallList(torusList);
	
	glUniform1i (texedobjloc, 0);
	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glCallList(spheresList); 
	glPopMatrix();
}

