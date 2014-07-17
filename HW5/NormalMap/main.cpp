#include "gl/glew.h"
#include "gl/gluit.h"
#include "gl/glpng.h"
#include "svl/svl.h"
#include <math.h>

#define CLAMP(v,lo,hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))
#define RADIUS 3
#define PI 3.141592653589

#pragma comment (lib, "glew32.lib")

int mainwin, viewer;

GLuint p, colortex, normaltex;
GLuint tan_loc, bitan_loc;
GLuint nobumploc;

int gw, gh;

bool NOBUMP = false;
float lowerBound  = 20.0;
float higherBound = 160.0;
float theta = 90.0;
float omega = 0.8;

void display(){}

void reshape(int w, int h)
{
	gw = w; gh = h;
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}

void light()
{
	float lightPos[] = {RADIUS * cos(theta * PI / 180.0), 3, RADIUS * sin(theta * PI / 180.0), 1};
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
	glColor3f(1, 1, 0);
	glutSolidSphere (0.1,12,12);
	glPopMatrix();
	glPopAttrib();

}

void grid()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	for (int i = -5; i <= 5; i++) {
		glVertex3i(-5, 0, i); glVertex3i(5, 0, i);
		glVertex3i( i, 0,-5); glVertex3i(i, 0, 5);
	}
	glEnd();
	glPopAttrib();
}

void wall()
{
	glBegin (GL_QUADS);
		// same attributes for every vertex
		glVertexAttrib3f(tan_loc,1,0,0);
		glVertexAttrib3f(bitan_loc, 0,1,0);
		glNormal3f(0,0,1);
		
		glMultiTexCoord2f(GL_TEXTURE0, 0,0); glVertex3i(-2, 0, 0); 
		glMultiTexCoord2f(GL_TEXTURE0, 1,0); glVertex3i(2,0,0);
		glMultiTexCoord2f(GL_TEXTURE0, 1,1); glVertex3i(2,4,0);
		glMultiTexCoord2f(GL_TEXTURE0, 0,1); glVertex3i(-2,4,0);
	glEnd();
}

void info()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	BEGIN_2D_OVERLAY (gw, gh);
	glColor3f(0, 1, 0);
	drawstr(10, 10, "Press b/B to use or not to use bump mapping.");
	if (NOBUMP) 
		drawstr(10, 40, "NO BUMP");
	else {
		glColor3f(0.5, 0, 0);
		drawstr(10, 40, "BUMP");
	}
	END_2D_OVERLAY();
	glPopAttrib();
}

void content()
{
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	glUseProgram(0);
	light();
	
	glUseProgram(p);
	wall();

	glUseProgram(0);
	grid();

	info();
	
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	if(key == 'b' || key == 'B') {
		NOBUMP ^= 1;
		glUseProgram(p);
		glUniform1i(nobumploc, NOBUMP);
	}

	glutPostRedisplay();
}

void timer(int dummy)
{
	theta += omega;
	
	theta = CLAMP(theta, lowerBound, higherBound);
	if(theta == lowerBound || theta == higherBound)
		omega *= -1;

	glutTimerFunc(10, timer, 0);
	ViewerRedisplay(viewer);
}

void init()
{
	glEnable (GL_DEPTH_TEST);
	glClearColor (.4,.4,.4,1);

	glewInit();

	extern GLuint setShaders(char*,char*);
	p = setShaders ("bumpmap.vert", "bumpmap.frag");
	pngSetStandardOrientation(1);
	colortex = pngBind("brick_wall.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	normaltex = pngBind("brick_wallDOT3.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_LINEAR, GL_LINEAR);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, normaltex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colortex);
	
	glUseProgram(p);
	glUniform1i(glGetUniformLocation(p,"NormalMap"), 0);
	glUniform1i(glGetUniformLocation(p,"ColorMap"),  1);

	tan_loc   = glGetAttribLocation(p, "Tangent");
	bitan_loc = glGetAttribLocation(p, "Bitangent");
	nobumploc = glGetUniformLocation(p, "nobump");
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow("Nomal Map.");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	
	viewer = ViewerNew (mainwin, 0,0,400,400, content);
	init();
	glutKeyboardFunc(keyboard);
	glutTimerFunc(10, timer, 0);
	glutMainLoop();
}
