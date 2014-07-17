#define MAIN "eyecroodinate"

#include <GL/glew.h>   // must be included before glut.h
#include <stdio.h>
#include <stdlib.h>

#include "GL/gluit.h"     // contains glut.h
#include "GL/glpng.h"
#include "svl/svl.h"

int mainwin, viewer;
int c1 = 0, c2 = 1;
bool origin = true;
bool reset = true;

#pragma comment(lib, "glew32.lib")

GLuint p;
int ww = 400, wh = 400;
int objeye = 1;
int selection = 0;
int obj = 0;
float ColorArray[3][4] = {1.0, 0.0, 0.0, 1.0,
						  0.0, 1.0, 0.0, 1.0,
						  0.0, 0.0, 1.0, 1.0};

char *obj_name[4] = {"bunny.obj", "toyplane.obj", "toycar.obj", "vwbus.obj"};

GLMmodel *m1;

void display(void){}
void reshape (int w, int h)
{
	ww = w;
	wh = h;
	glViewport (0,0,w,h);
	ViewerReshape (viewer);
}

void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(.4, .4, .4, 1); 

	glewInit();

	if(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else 
	{
		printf("No GLSL support\n");
		exit(1);
	}

	char vsh[20], fsh[20];
	sprintf (vsh, "%s.vert", MAIN);
	sprintf (fsh, "%s.frag", MAIN);

	extern GLuint setShaders(char*,char*);
	p = setShaders(vsh, fsh);

	glUseProgram(p);
	
	glUniform4fv(glGetUniformLocation(p, "color_positive"), 1, ColorArray[c1%3]);
	glUniform4fv(glGetUniformLocation(p, "color_negative"), 1, ColorArray[c2%3]);

	glUniform1i(glGetUniformLocation(p, "type"), selection);
	glUniform1i(glGetUniformLocation(p, "objeye"), objeye);

	m1 = glmReadOBJ(obj_name[0]);
	glmUnitize(m1);
}

void showOrigin()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glLineWidth(3.0);
	glBegin(GL_LINES);
	glColor3f(1., 0., 0.);
	glVertex3i(0, 0, 0);	glVertex3i(2, 0, 0);
	glColor3f(0., 1., 0.);
	glVertex3i(0, 0, 0);	glVertex3i(0, 2, 0);
	glColor3f(0., 0., 1.);
	glVertex3i(0, 0, 0);	glVertex3i(0, 0, 2);
	glEnd();
	glPopAttrib();
}

void resetCameraPosition()
{
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,5, 0,0,0, 0,1,0);
	reset = false;
}

void helper()
{
	BEGIN_2D_OVERLAY (ww, wh);
	setfont("9x15", 80);
	
	drawstr(10, 10, "o/O = Show origin.");
	drawstr(10, 30, "r/R = Reset camera position.");
	drawstr(10, 50, "+/- = Change the color in positive/negative halves.");
	drawstr(10, 70, "x/X = Coordinate selection (X or Y).");
	drawstr(10, 90, "c/C = Change the model.");

	END_2D_OVERLAY();
}

void content (void)
{
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if( origin )
	{
		glUseProgram(0);
		showOrigin();
	}

	glUseProgram(p);
	if( reset )
		resetCameraPosition();

	helper();
	
	glmDraw (m1, GLM_SMOOTH | GLM_MATERIAL);

	glutSwapBuffers();
}

void keyboard (unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'o': case'O':
			origin ^= 1;
			break;

		case 'r': case'R':
			reset ^= 1;
			break;

		case '+':
			c1 += 1;
			glUniform4fv(glGetUniformLocation(p, "color_positive"), 1, ColorArray[c1%3]);
			break;

		case '-':
			c2 += 1;
			glUniform4fv(glGetUniformLocation(p, "color_negative"), 1, ColorArray[c2%3]);
			break;

		case 'x': case 'X':
			selection += 1;
			glUniform1i(glGetUniformLocation(p, "type"), selection%3);
			break;

		case 'c': case 'C':
			obj += 1;
			m1 = glmReadOBJ(obj_name[obj%4]);
			glmUnitize(m1);
			break;

		case '.':
			if(objeye == 1)
				objeye = 2;
			else
				objeye = 1;
			glUniform1i(glGetUniformLocation(p, "objeye"), objeye);
			break;
	}

	glutPostRedisplay();
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow (MAIN);
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);
	viewer = ViewerNew (mainwin,0,0,400,400, content);

	printf ("%s %s\n", glGetString (GL_VERSION),
					   glGetString (GL_SHADING_LANGUAGE_VERSION));

	init();

	glutKeyboardFunc(keyboard);

	glutMainLoop();
}