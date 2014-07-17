#define MAIN "passthru"

#include <GL/glew.h>   // must be included before glut.h
#include <stdio.h>
#include <stdlib.h>

#pragma comment (lib, "glew32.lib")

#include "gl/gluit.h"     // contains glut.h

int mainwin, viewer;

void content (void)
{
	glColor3ub (255,0,0);
	glClearColor (.3,.3,.3,1);

	glBegin (GL_LINES);
	//glVertex2i (1,0);
	//glVertex2i (3,0);
	glVertex2i (-2,0);
	glVertex2i (2,0);
	//glVertex2i (-2,3);
	//glVertex2i (2,3);
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
	glClearColor(.4, .4, .4, 1); 

	glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GLEW_EXT_geometry_shader4)
		printf("Ready for GLSL\n");
	else {
		printf("No GLSL support\n");
		exit(1);
	}

	GLuint p;
	char vsh[20], fsh[20],gsh[20];
	sprintf (vsh, "%s.vert", MAIN);
	sprintf (fsh, "%s.frag", MAIN);
	sprintf (gsh, "%s.geom", MAIN);

	extern GLuint setVFGShaders (char*,char*,char*);

	p = setVFGShaders(vsh, fsh,gsh);
	////////////////////////////////
	glProgramParameteriEXT(p,GL_GEOMETRY_INPUT_TYPE_EXT,GL_LINES);
	glProgramParameteriEXT(p,GL_GEOMETRY_OUTPUT_TYPE_EXT,GL_LINE_STRIP);

	int temp;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
	glProgramParameteriEXT(p,GL_GEOMETRY_VERTICES_OUT_EXT,temp);
	////////////////////////

	glLinkProgram(p); // POSTponed linking

	glUseProgram (p);
	
}


int main (int argc, char** argv)
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

