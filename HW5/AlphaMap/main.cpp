#include "gl/glew.h"
#include "gl/gluit.h"
#include "gl/glpng.h"
#include <math.h>

#define CLAMP(v,lo,hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))
#define RADIUS 8
#define PI 3.141592653589

#pragma comment (lib, "glew32.lib")

int mainwin, viewer;
int gw, gh;

GLuint prog;
GLuint alphatex, walltex, wall_normal;
GLuint floortex, floor_normal;

GLuint tan_loc, bitan_loc;
GLuint nobumploc;
GLuint holeflag;

bool NOBUMP = false;

float lowerBound  = 0.0;
float higherBound = 180.0;
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
	float lightPos[] = {1 + RADIUS * cos(theta * PI / 180.0), 4, 1 + RADIUS * sin(theta * PI / 180.0), 1};
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

void axis()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glLineWidth(5.0);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3i(0, 0, 0); glVertex3i(3, 0, 0);
	glColor3f(0, 1, 0);	glVertex3i(0, 0, 0); glVertex3i(0, 3, 0);
	glColor3f(0, 0, 1);	glVertex3i(0, 0, 0); glVertex3i(0, 0, 3);
	glEnd();
	glPopAttrib();
}

void floor()
{
	glBegin(GL_QUADS);
		// same attributes for every vertex
		glVertexAttrib3f(tan_loc,1,0,0);
		glVertexAttrib3f(bitan_loc, 0,0,-1);
		glNormal3f(0,1,0);
		
		glMultiTexCoord2f(GL_TEXTURE0, 0,0); glVertex3i(-3, 0,-3); 
		glMultiTexCoord2f(GL_TEXTURE0, 1,0); glVertex3i( 3, 0,-3);
		glMultiTexCoord2f(GL_TEXTURE0, 1,1); glVertex3i( 3, 0, 3);
		glMultiTexCoord2f(GL_TEXTURE0, 0,1); glVertex3i(-3, 0, 3);
	glEnd();
}

void wall()
{
	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0, 0,0); glVertex3i(-3, 0, 0); 
		glMultiTexCoord2f(GL_TEXTURE0, 1,0); glVertex3i( 3, 0, 0);
		glMultiTexCoord2f(GL_TEXTURE0, 1,1); glVertex3i( 3, 6, 0);
		glMultiTexCoord2f(GL_TEXTURE0, 0,1); glVertex3i(-3, 6, 0);
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
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	glUseProgram(0);
	light();
	//axis();

	glUseProgram(prog);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floortex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, floor_normal);
	glUniform1i(holeflag, false);

	glPushMatrix();
	glTranslatef(0, 0, 3);
	floor();
	glPopMatrix();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, walltex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, wall_normal);

	glUniform1i(holeflag, false);
	glVertexAttrib3f(tan_loc, 1,0,0);
	glVertexAttrib3f(bitan_loc, 0,-1,0);
	glNormal3f(0,0,1);
	wall();

	glUniform1i(holeflag, true);
	glVertexAttrib3f(tan_loc, 0,0,1);
	glVertexAttrib3f(bitan_loc, 0,1,0);
	glNormal3f(1,0,0);
	glPushMatrix();
	glTranslatef (3,0,3);
	glRotatef(-90, 0,1,0);
	wall();
	glPopMatrix();

	glUseProgram(0);
	info();

	glutSwapBuffers();
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

void keyboard(unsigned char key, int x, int y)
{
	if(key == 'b' || key == 'B') {
		NOBUMP ^= 1;
		glUseProgram(prog);
		glUniform1i(nobumploc, NOBUMP);
	}

	glutPostRedisplay();
}

void init()
{
	glEnable (GL_DEPTH_TEST);
	glClearColor (.4,.4,.4,1);

	pngSetStandardOrientation(1);

	floortex	 = pngBind("pebble.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	floor_normal = pngBind("pebbleDOT3.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	
	walltex		= pngBind("brick_wall.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	wall_normal = pngBind("brick_wallDOT3.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	
	alphatex = pngBind("alphamask.png", PNG_NOMIPMAP, PNG_ALPHA, NULL, GL_CLAMP, GL_NEAREST, GL_NEAREST);

	glewInit(); 

	extern GLuint setShaders(char*,char*);
	prog = setShaders("bumpmapWithHole.vert", "bumpmapWithHole.frag");
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, alphatex);

	glUseProgram(prog);
	tan_loc   = glGetAttribLocation(prog, "Tangent");
	bitan_loc = glGetAttribLocation(prog, "Bitangent");

	holeflag  = glGetUniformLocation(prog, "dighole");
	nobumploc = glGetUniformLocation(prog, "nobump");

	glUniform1i(glGetUniformLocation(prog, "ColorMap"),  0);
	glUniform1i(glGetUniformLocation(prog, "NormalMap"), 1);
	glUniform1i(glGetUniformLocation(prog, "alphamap"),  2);
	
	glUniform1i(nobumploc, NOBUMP);
}

void main(int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("Dig a hole.");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew(mainwin, 0,0,400,400, content);
	init();
	glutTimerFunc(10, timer, 0);
	glutKeyboardFunc(keyboard);
	ViewerExtent (viewer, 10.0);
	glutMainLoop();
}