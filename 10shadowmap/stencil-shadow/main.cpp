#include "gl/gluit.h"
#include "gl/glpng.h"

int viewer, mainwin;
void display (void) {}
void reshape (int w, int h) 
{
    glViewport (0,0,w,h);
    ViewerReshape (viewer);
}

//////////////////////////////////////////////////////////////////////
//// Stencil Shadow Stuff

// light position
static float LightPosition[] = {2.0f, 3.1f, 2.0f, 1.0f};
// shadow matrix
static float fShadowMatrix[16];
static float Plane[4] = {0,1,0,1};			// ax + by + cz + d = 0, [a,b,c] normalized

void SetShadowMatrix(float fDestMat[16],float fLightPos[4],float fPlane[4])
{
	float dot;

	// dot product of plane and light position
	dot =	fPlane[0] * fLightPos[0] + 
			fPlane[1] * fLightPos[1] + 
			fPlane[2] * fLightPos[2] + 
			fPlane[3] * fLightPos[3];

	// first column
	fDestMat[0] = dot - fLightPos[0] * fPlane[0];
	fDestMat[4] = 0.0f - fLightPos[0] * fPlane[1];
	fDestMat[8] = 0.0f - fLightPos[0] * fPlane[2];
	fDestMat[12] = 0.0f - fLightPos[0] * fPlane[3];

	// second column
	fDestMat[1] = 0.0f - fLightPos[1] * fPlane[0];
	fDestMat[5] = dot - fLightPos[1] * fPlane[1];
	fDestMat[9] = 0.0f - fLightPos[1] * fPlane[2];
	fDestMat[13] = 0.0f - fLightPos[1] * fPlane[3];

	// third column
	fDestMat[2] = 0.0f - fLightPos[2] * fPlane[0];
	fDestMat[6] = 0.0f - fLightPos[2] * fPlane[1];
	fDestMat[10] = dot - fLightPos[2] * fPlane[2];
	fDestMat[14] = 0.0f - fLightPos[2] * fPlane[3];

	// fourth column
	fDestMat[3] = 0.0f - fLightPos[3] * fPlane[0];
	fDestMat[7] = 0.0f - fLightPos[3] * fPlane[1];
	fDestMat[11] = 0.0f - fLightPos[3] * fPlane[2];
	fDestMat[15] = dot - fLightPos[3] * fPlane[3];
}

void ShowLight()
{
	glPushAttrib (GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix(); 
	glTranslatef (LightPosition[0],LightPosition[1],LightPosition[2]);
	glColor3ub (255,255,0); glutSolidSphere (0.1,12,12);
	glPopMatrix();
	glPopAttrib();
}


extern void DrawFloor (float,float,float);
extern void DrawOccluder();

void content (void)
{
	glClear (GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	////////////////////////////////////////////////////
	// 1. set up color & stencil buffers of floor
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
				
	// render the plane which the shadow will be on
	// color and depth buffer are disabled, only the stencil buffer
	// will be modified
	DrawFloor(0,0.5,0);

	// turn the color and depth buffers back on
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);


	// until stencil test is diabled, only write to areas where the
	// stencil buffer has a one. This is to draw the shadow only on
	// the floor.
	glStencilFunc(GL_EQUAL, 1, 0xFF);

	// don't modify the contents of the stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	DrawFloor(0,0.5,0);


	////////////////////////////////////////////////////
	// 2. set up Shadow matrix based on light position
	extern void get_scene_light_pos(float*);
	get_scene_light_pos (LightPosition);
	SetShadowMatrix(fShadowMatrix, LightPosition, Plane);

	////////////////////////////////////////////////////
	// 3. draw the shadow of the occluder
	glPushAttrib (GL_ENABLE_BIT);
	glPushMatrix();
	// draw the shadow as black, blended with the surface, with no lighting, and not
	// preforming the depth test
		glColor4f(0.0, 0.0, 0.0, 0.5f);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		// make sure that we don't draw at any raster position more than once. Using GL_INCR
		// here is smart because the value of the area in stencil buffer wich will be drawn to
		// increases to 2, and earlier we have set the stencil buffer function to only write to
		// areas with a one. This means that a shadow will not be draw 2 times on the same area!
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		// project the cube through the shadow matrix
		glMultMatrixf(fShadowMatrix);
		DrawOccluder ();

		//glEnable(GL_TEXTURE_2D);
		//glEnable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);
		//glEnable(GL_LIGHTING);
	glPopMatrix();
	glPopAttrib();

	glDisable(GL_STENCIL_TEST);


	////////////////////////////////////////////////////
	// 4. draw the (real) occluder 
	DrawOccluder();


	////////////////////////////////////////////////////
	// 5. draw the light
	ShowLight ();

	glutSwapBuffers();
}

void init (void)
{
	glClearColor(.4, .4, .4, 1);

	float LightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
	float LightDiffuse[] = {1.0, 1.0, 1.0, 1.0}; 
	float LightSpecular[] = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable (GL_NORMALIZE);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	extern void initOccluder(), initFloor(char*);
	initOccluder();
	initFloor("marble.png");		
}

void keyboard (unsigned char key, int x, int y)
{
	extern void scene_keys (unsigned char);
	scene_keys (key);

	glutPostRedisplay();
}

void timer (int dummy)
{
	extern void scene_timer ();
	scene_timer();

	glutTimerFunc (10, timer, 0);
	glutPostRedisplay();
}

void main (int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE |GLUT_RGB|GLUT_DEPTH|GLUT_STENCIL);
    glutInitWindowSize (400,400);
    
    mainwin = glutCreateWindow ("Stencil Shadow [xX]");
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    viewer = ViewerNew (mainwin, 0,0,400,400, content);
    init();
	glutKeyboardFunc (keyboard);
	glutTimerFunc (10, timer, 0);

	glutMainLoop();
}

    