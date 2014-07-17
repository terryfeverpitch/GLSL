#include <stdlib.h>
#include <glut.h>

#include "pbm.h"

/////////////////////////////////////////////////////////
// static (Global) Value
static GLsizei wd, hi;//width, height
static int line_mat, ball_mat;

static ParticleSystem p;
///////////////////////////////////////////////////////////

void drawOneLine(const Vec3 &p1, const Vec3 &p2)
{    
	glBegin(GL_LINES);
        glVertex3dv (p1.Ref()); glVertex3dv (p2.Ref());
    glEnd();
}

void ShowModel (ParticleSystem p)
{
    glCallList (line_mat);    
	for(unsigned int i = 0; i < p->springs.size(); i++)
		drawOneLine(p->springs[i]->p1->x, p->springs[i]->p2->x);

	glCallList (ball_mat);    
	for (unsigned int j = 0; j < p->particles.size(); j++) {
        glPushMatrix();
			glTranslated (p->particles[j]->x[0], p->particles[j]->x[1], p->particles[j]->x[2]);
			glutSolidSphere(0.2, 20, 20);
        glPopMatrix();
    }
}
///////////////////////////////////////////////////////////////////////////////

double min_x = -5;
double max_x = 5;
double min_y = -5;
double max_y = 5;

void MouseToWorld (const int mous_x, const int mous_y, Vec3 &x)
{
     x[0] = (max_x - min_x) * mous_x/wd + min_x;
	 x[1] = (double)(hi/2 - mous_y)/hi * (max_y - min_y);
	 x[2] = 0.0;
}

void  processDown (const int mous_x, const int mous_y)
{
    double min_dis = 1.0e+16, dis;
    int pulled;
	Vec3 mouse_world;

	MouseToWorld (mous_x, mous_y, mouse_world);

    // find closest particle
    for (unsigned int i = 0; i < p->particles.size(); i++) {
		if (p->particles[i]->anchor != 1) {    // do not pull anchored particle
			dis = len (p->particles[i]->x - mouse_world);
			if(dis < min_dis){
				min_dis = dis;
				pulled = i;
			}
		}
	}

	p->mouse_loc = mouse_world;
	p->pulled = pulled;
} 

void processUp(void)
{
    p->pulled = -1;
}
void mouse (int button, int state, int x, int y)
{
     if (button == GLUT_LEFT_BUTTON) {
	    if (state == GLUT_DOWN)
	        processDown (x,y);
	    else if (state == GLUT_UP)
	        processUp ();
     }
}

void motion (int mous_x, int mous_y)
{
    processDown (mous_x, mous_y); 
}

//////////////////////////////////////////////////////////////////////////

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShowModel (p);
	
	// if pulled, draw the mouse spring
	glCallList (line_mat);
	glColor3f (1,1,0);
    if (p->pulled >= 0) {
		drawOneLine(p->mouse_loc, p->particles[p->pulled]->x);
    }
	
    glutSwapBuffers();
}


void timer (int dummy)
{
	float step = 10./1000;  // 10 msec

	EulerStep (p, step);
	extern void CollisionDriver (ParticleSystem,double);
	
	CollisionDriver (p, step);
	
	glutTimerFunc (10, timer, 0);
	glutPostRedisplay();
}

void myinit (void) 
{
	line_mat = glGenLists(1);
    glNewList (line_mat, GL_COMPILE);
		glDisable(GL_DEPTH_TEST);   // important !!!
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x00ff);
		glLineWidth(5.0f);  // modify: variable line width
		glColor3f (1,1,1);
	glEndList();

	ball_mat = glGenLists(1);
	glNewList (ball_mat, GL_COMPILE);
		glColor3f (1,0,0);
		glEnable(GL_DEPTH_TEST);
    glEndList();

	glShadeModel (GL_SMOOTH);
    glClearColor (0.0, .2, .0, 1.);
  
	///////////////////////////////////////////////
	/// modeling ...
	extern ParticleSystem InitModel0(), InitModel();
	
	p = InitModel();   // two-particle one-link model
}


void reshape(GLsizei w, GLsizei h)
{
    wd = w; hi = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D (min_x, max_x, min_y, max_y);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard (unsigned char key, int x, int y)
{
	static int gravity_on = 1;
	switch (key) {
	case 'g': case 'G':
		gravity_on ^= 1;
		break;
	case 27:
		exit(1);
		break;
	}

	if (gravity_on) {
		glutSetWindowTitle ("G [on]");
		SetGravity (50.0);
	} else {
		glutSetWindowTitle ("G [off]");
		SetGravity (0.0);
	}

}

int main(int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    glutCreateWindow ("spring: Press G");

    myinit ();

    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

	glutTimerFunc (10, timer, 0);
	
	glutKeyboardFunc(keyboard);

    glutMainLoop();
    return(0);
}
