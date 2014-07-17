#define MAIN "test1"

#include <GL/glew.h>   // must be included before glut.h
#include <stdio.h>
#include <stdlib.h>

#include "GL/gluit.h"     // contains glut.h

int mainwin, viewer;
GLuint p;
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
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("No GLSL support\n");
		exit(1);
	}

	char vsh[20], fsh[20];
	sprintf (vsh, "%s.vert", MAIN);
	sprintf (fsh, "%s.frag", MAIN);

	

	extern GLuint setShaders (char*,char*);
	p = setShaders(vsh, fsh);
	
	//
	// demonstrate uniform variables
	// 
	glUseProgram (p);
	int colorloc = glGetUniformLocation (p, "mycolor");
	glUniform3f (colorloc, 1.0, 1.0, 0.0);
}

void content (void)
{
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glutSolidTeapot (1.0);

	glutSwapBuffers();
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
    //
	// 4.3.0 4.30 NVIDIA via Cg compiler
	//
	init();

	glutMainLoop();
}

