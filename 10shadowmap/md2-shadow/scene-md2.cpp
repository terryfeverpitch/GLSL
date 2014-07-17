#include <gl/glew.h>
#include <gl/glut.h>
#include "gl/glpng.h"

#include "md2m.h"

static t3DModel g_3DModel;
static float center[3], size[3];
static float scale;

#define STAND 0
#define JUMP 6
static int gpose = STAND;
static int loopOver;


void initMD2M (void)
{
#define max(x,y) ((x) > (y)? (x) : (y))
	MD2MLoad (&g_3DModel, "yoshi-tris.md2", "yoshi.png");

	 MD2MSetAnimation (&g_3DModel, STAND);
	 MD2MSetLoop (&g_3DModel, GL_TRUE);
	 MD2MSetAnimationSpeed (&g_3DModel, 5.0); 
  	
	 MD2MFindDimension (&g_3DModel, center, size);
	 scale = max (max (size[0], size[1]), size[2]);
}

static int floorTid;
void initScene(int program)
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glActiveTexture (GL_TEXTURE0);
	pngSetStandardOrientation (1);
	floorTid = pngBind("marble.png", PNG_BUILDMIPMAPS, PNG_SOLID, NULL, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	// sceneTid not used
	
	int sceneMaploc = glGetUniformLocation (program, "sceneMap");
	glUniform1i (sceneMaploc, 0);

	initMD2M();
}


static void DrawFloor(float fCenterX, float fCenterY, float fCenterZ)
{
	float FloorColor[] = {1.,1.,1.,1.};
	glMaterialfv(GL_FRONT, GL_SPECULAR, FloorColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, FloorColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 200.0);

	glActiveTexture(GL_TEXTURE0);
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
	DrawFloor(0,0,0);
	
	glPushMatrix();
		glTranslatef (0,1,0);
		glScalef (2/scale, 2/scale, 2/scale);
		glTranslatef (-center[0], -center[1], -center[2]);
		loopOver = MD2MInterpolate (&g_3DModel);
	glPopMatrix();	
}

static float lightPosition[4] = {-1,3,3,1};
void scene_keys (unsigned char key) 
{
    switch (key) {
	case 'j': case 'J': // jump   
		gpose = JUMP;
		MD2MSetAnimation (&g_3DModel, gpose);
		MD2MSetLoop (&g_3DModel, GL_FALSE);
		break;
	case 'X':
		lightPosition[0] += .5;
		break;
	case 'x':
		lightPosition[0] -= .5;
		break;
	}
}

void get_scene_light_pos (float* pos)
{
	for (int i = 0; i < 4; i++)
		pos[i] = lightPosition[i];
}

void scene_timer ()
{
   if (loopOver && gpose == JUMP) {
		// always resume stand (or previous action) after jump ...
		gpose = STAND;
		MD2MSetAnimation (&g_3DModel, gpose);
		MD2MSetLoop (&g_3DModel, GL_TRUE);
	}		
}

