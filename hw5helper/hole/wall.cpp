#include <gl/glew.h>
#include "gluit.h"
#include "gl/glpng.h"

#pragma comment (lib, "glew32.lib")

int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}

GLuint p;

void grid()
{
	glBegin (GL_LINES);
	for (int i = -10; i <= 10; i++) {
		glVertex3i (-10,0,i), glVertex3i (10,0,i);
		glVertex3i (i,0,-10), glVertex3i (i,0,10);
	}
	glEnd();
}
void wall()
{
	glBegin (GL_QUADS);
		glMultiTexCoord2f (GL_TEXTURE0, 0,0); glVertex3i (-2, 0, 0); 
		glMultiTexCoord2f (GL_TEXTURE0, 1,0); glVertex3i (2,0,0);
		glMultiTexCoord2f (GL_TEXTURE0, 1,1); glVertex3i (2,4,0);
		glMultiTexCoord2f (GL_TEXTURE0, 0,1); glVertex3i (-2,4,0);
	glEnd();
}

GLuint holeflag;

void content()
{
	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	glUseProgram(0);
	grid();

	glUseProgram(p);
	glUniform1i (holeflag, 0);
	wall();
	glUniform1i (holeflag, 1);
	glPushMatrix();
	glTranslatef (2,0,2);
	glRotatef (-90, 0,1,0);
	wall();
	glPopMatrix();

	glutSwapBuffers();
}
GLuint alphatex, walltex;

void init()
{
	glEnable (GL_DEPTH_TEST);
	glClearColor (.4,.4,.4,1);

	pngSetStandardOrientation(1);
	walltex = pngBind("brick.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	alphatex = pngBind("alphamask.png", PNG_NOMIPMAP, PNG_ALPHA, NULL, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	glewInit(); 

	extern GLuint setShaders(char*,char*);
	p = setShaders ("hole.vert", "hole.frag");

	// set texture object in each texture unit

	// weird ... TU0 & TU1
	glActiveTexture (GL_TEXTURE2);
	glBindTexture (GL_TEXTURE_2D, walltex);

	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, alphatex);

	glUseProgram(p);  //// key .....
	// set uniforms
	glUniform1i (glGetUniformLocation (p, "wallmap"), 2);
	glUniform1i (glGetUniformLocation (p, "alphamap"), 0);
	holeflag = glGetUniformLocation (p, "dighole");
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("dig a hole");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew (mainwin, 0,0,400,400, content);
	init();
	ViewerExtent (viewer, 10.0);
	glutMainLoop();
}
