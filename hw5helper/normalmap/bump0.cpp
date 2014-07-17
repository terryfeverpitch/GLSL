#include <gl/glew.h>
#include "gl/gluit.h"
#include "gl/glpng.h"
#include "svl/svl.h"

#pragma comment (lib, "glew32.lib")

int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}
GLuint p, colortex, normaltex;
GLuint tan_loc, bitan_loc;

void grid()
{
	glPushAttrib (GL_ENABLE_BIT);
	glDisable (GL_TEXTURE_2D);
	glDisable (GL_LIGHTING);
	glColor3f(1,1,1);
	glBegin (GL_LINES);
	for (int i =-5; i <= 5; i++) {
		glVertex3i (-5, 0, i); glVertex3i (5,0,i);
		glVertex3i (i,0,-5); glVertex3i(i,0,5);
	}
	glEnd();
	glPopAttrib();
}
GLuint nobumploc;
int bumptoggle;

void wall()
{
	glBegin (GL_QUADS);
		// same attributes for every vertex
		glVertexAttrib3f(tan_loc,1,0,0);
		glVertexAttrib3f(bitan_loc, 0,1,0);
		glNormal3f (0,0,1);
		
		glMultiTexCoord2f (GL_TEXTURE0, 0,0); glVertex3i (-2, 0, 0); 
		glMultiTexCoord2f (GL_TEXTURE0, 1,0); glVertex3i (2,0,0);
		glMultiTexCoord2f (GL_TEXTURE0, 1,1); glVertex3i (2,4,0);
		glMultiTexCoord2f (GL_TEXTURE0, 0,1); glVertex3i (-2,4,0);
	glEnd();
}

float theta;
void content()
{
	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	float pos[] = {0,3,3,1};
	glPushMatrix();
	glLightfv (GL_LIGHT0, GL_POSITION, pos);
	glPopMatrix();

	glUseProgram(p);
	wall();

	glUseProgram(0);
	grid();
	
	glutSwapBuffers();
}


void init()
{
	glEnable (GL_DEPTH_TEST);
	glClearColor (.4,.4,.4,1);

	glewInit();
	extern GLuint setShaders(char*,char*);
	p = setShaders ("bumpmap.vert", "bumpmap.frag");
	pngSetStandardOrientation(1);
	colortex = pngBind("rock.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	normaltex = pngBind("rockDOT3.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_LINEAR, GL_LINEAR);

	glUseProgram (p);
	glUniform1i (glGetUniformLocation (p,"NormalMap"), 0);
	glUniform1i (glGetUniformLocation (p,"ColorMap"), 1);

	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, normaltex);
	glActiveTexture (GL_TEXTURE1);
	glBindTexture (GL_TEXTURE_2D, colortex);
	
	tan_loc = glGetAttribLocation (p,"Tangent");
	bitan_loc = glGetAttribLocation (p,"Bitangent");	
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("bump (b,SPC)");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew (mainwin, 0,0,400,400, content);
	init();

	glutMainLoop();
}
