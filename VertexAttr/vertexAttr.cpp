#define MAIN "vertexAttr"

/*
  Simple Demo for GLSL
  www.lighthouse3d.com
*/

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment (lib, "glew32.lib")

#include "gluit.h"

int mainwin, viewer;

int temp_loc;
void content (void)
{
	glColor3ub (255,0,0);

	// how about vector attribute
	glBegin(GL_TRIANGLE_STRIP);

		glVertexAttrib1f(temp_loc,60);
		glVertex2f(-1,1);

		glVertexAttrib1f(temp_loc,20);
		glVertex2f(1,1);

		glVertexAttrib1f(temp_loc,10);
		glVertex2f(-1,-1);

		glVertexAttrib1f(temp_loc,95);
		glVertex2f(1,-1);

	glEnd();

	glutSwapBuffers();
}

void display(void){}
void reshape (int w, int h)
{
	glViewport (0,0,w,h);
	ViewerReshape (viewer);
}

void init (void)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);

	glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("No GLSL support\n");
		exit(1);
	}

	GLuint p;
	char vsh[20], fsh[20];
	sprintf (vsh, "%s.vert", MAIN);
	sprintf (fsh, "%s.frag", MAIN);
	extern GLuint setShaders (char*,char*);

	p = setShaders(vsh, fsh);;

	glUseProgram (p);

	temp_loc = glGetAttribLocation (p,"temperature");

	glUniform1f (glGetUniformLocation (p, "cool_temp"), 0);
	glUniform1f (glGetUniformLocation (p, "hot_temp"), 100);
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

	init();

	glutMainLoop();
}

