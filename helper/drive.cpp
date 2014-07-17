#include "gluit.h"
#include "svl/svl.h"

int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}

void grid()
{
	glPushAttrib (GL_ENABLE_BIT);
	glDisable (GL_LIGHTING);
	glDisable (GL_TEXTURE_2D);
	glColor3f (1,1,1);
	glBegin (GL_LINES);
	for (int i = -10; i <= 10; i++) {
		glVertex3i (-10,0,i); glVertex3i (10,0,i);
		glVertex3i (i,0,-10); glVertex3i (i,0,10);
	}
	glEnd();
	glPopAttrib();
}

Vec3 point;
double angle;
double speed;

void content()
{
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	grid();

	glColor3f (1,0,0);
	glPointSize (20);
	glBegin (GL_POINTS);
	glVertex3dv (point.Ref());
	glEnd();
#if 0
	glPushMatrix();
	glTranslated (point[0], point[1],point[2]);
	glRotatef (angle, 0,1,0);
	glutWireTeapot (0.3);
	glPopMatrix();
#endif

	glutSwapBuffers();
}

void special (int key, int x, int y)
{
#define CLAMP(v,lo,hi)  ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))
	switch (key) {
		case GLUT_KEY_UP:
			speed += 0.1;
		break;
		case GLUT_KEY_DOWN:
			speed -= 0.1;
		break;
		case GLUT_KEY_RIGHT:
			angle -= 2;
		break;
		case GLUT_KEY_LEFT:
			angle += 2;
		break;
	}
	speed = CLAMP (speed, 0.0, 3.0);
}


void timer (int dummy)
{
	glutTimerFunc (100, timer, 0);
	point += Rot3 (Vec3(0,1,0),angle/180*vl_pi) * Vec3(1,0,0)*speed/10;	

	glutPostRedisplay();
}


void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("drive [ARROW]");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew (mainwin, 0,0,400,400, content);

	glutSpecialFunc (special);
	glutTimerFunc (10, timer, 0);

	glutMainLoop();
}
