
#include "gluit.h"
#include "gl/glpng.h"
#include "svl/svl.h"


GLMmodel *al;

int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}


GLuint floortex;

void floor()
{
	glPushAttrib (GL_ALL_ATTRIB_BITS);
	glBindTexture (GL_TEXTURE_2D, floortex);
	glEnable (GL_TEXTURE_2D);
	glDisable (GL_LIGHTING);

	glBegin (GL_QUADS);
	glTexCoord2f (0,0); glVertex3i (-10,0,10); 
	glTexCoord2f (10,0); glVertex3i (10,0,10);
	glTexCoord2f (10,10); glVertex3i (10,0,-10); 
	glTexCoord2f (0,10); glVertex3i (-10,0,-10);
	glEnd();
	glPopAttrib();
}

void cone (float base, float height, float x, float z)
{
	glPushMatrix();
	glTranslatef (x,0,z);
	glRotatef (-90,1,0,0);
	glutSolidCone (base, height, 20,20);
	glPopMatrix();
}

void scene()
{
	floor();

	float red[] = {1,0,0,1};
	float yellow[] = {.8,.8,0,1};
	float purple[] = {.4,0,.4,1};
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
	cone (.4,1.2, -2,3);
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
	cone (.4,1.6, 2,-2);
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, purple);
	cone (.4,1.2, -2.4,-2.5);
}

double theta;

void content()
{
	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	scene();

	glPushMatrix();
	float r = 1.5;
	glTranslatef (r*cos(theta*vl_pi/180), 0, -r*sin(theta*vl_pi/180));
	glRotatef (theta+180, 0,1,0);
	glTranslatef (0,1,0);
	glmDraw (al, GLM_MATERIAL|GLM_SMOOTH);

	// compute head window coordinate
	// Al's head is about (0,1,0) world coordinate
	// NDC = proj ( [PJ][MV](head) )
	double winx, winy, winz;
	
	double mv[16], proj[16];
	int viewport[4];
	glGetDoublev (GL_MODELVIEW_MATRIX, mv);
	glGetDoublev (GL_PROJECTION_MATRIX, proj);
	glGetIntegerv (GL_VIEWPORT, viewport);
	gluProject (0,0.9,0, mv, proj, viewport, &winx, &winy, &winz);

	// draw something overhead
	glPushAttrib (GL_ENABLE_BIT);
	glDisable (GL_DEPTH_TEST); glDisable (GL_LIGHTING);
	glMatrixMode (GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D (0,400,0,400);  
	glMatrixMode (GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
	
	glPointSize (30);
	glBegin (GL_POINTS);
	glVertex2d (winx, winy);
	glEnd();

	glPopMatrix();

	glMatrixMode (GL_PROJECTION); glPopMatrix();
	glMatrixMode (GL_MODELVIEW); glPopMatrix();
	glPopAttrib();
	
	glutSwapBuffers();
}

int turn;
void timer (int dummy)
{
	glutTimerFunc (10, timer, 0);
	glutPostRedisplay();
	if (turn) theta += 2;
}

void init()
{
	glClearColor (.4,.4,.4,1);
	glEnable (GL_DEPTH_TEST);

	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	
	al = glmReadOBJ ("al.obj");
	glmUnitize (al);
	glmFacetNormals (al);
	glmVertexNormals (al, 90.0);
	
	floortex = pngBind("marble.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_REPEAT, GL_NEAREST, GL_NEAREST);
}

void keyboard (unsigned char key, int x, int y)
{
	if (key == ' ')
		turn ^= 1;
	glutPostRedisplay();
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("Part II");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew (mainwin, 0,0,400,400, content);
	init();
	ViewerExtent (viewer, 8.0);
	glutTimerFunc (10, timer, 0);
	glutKeyboardFunc (keyboard);

	glutMainLoop();
}
