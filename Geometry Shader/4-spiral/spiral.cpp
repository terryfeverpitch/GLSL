#include <stdio.h>			//C standard IO
#include <stdlib.h>			//C standard lib
#include <string.h>			//C string lib

#include <GL/glew.h>			//GLEW lib
#include "gluit.h"			//GLUT lib

#pragma comment (lib, "glew32.lib")

GLuint p;			//Handlers for our vertex, geometry, and fragment shaders
int mainwin, viewer;

void display(){}
void reshape (int w, int h)
{
	glViewport (0,0,w,h);
	ViewerReshape(viewer);
}

void axes()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glLineWidth (5.0);
	glUseProgram(0);
	glBegin(GL_LINES);
	glColor3f (1,0,0);glVertex2i(0,0); glVertex3i(1,0,0);
	glColor3f(0,1,0); glVertex2i(0,0); glVertex3i(0,1,0);
	glColor3f (0,0,1);glVertex2i(0,0); glVertex3i(0,0,1);
	glEnd();
	glPopAttrib();
}
#define LEN 3
void content (void) 
{
	glClear(GL_COLOR_BUFFER_BIT);

	axes();
	glUseProgram(p);
	glBegin(GL_LINES);
		glVertex3i(1,0,1);
		glVertex3i(2,3,0);
	glEnd();

	glutSwapBuffers();
}

void init() 
{	
	extern GLuint setVFGShaders(char*,char*,char*);

	p = setVFGShaders ("spiral.vert", "spiral.frag", "spiral.geom");

	////////////////////////////////
	glProgramParameteriEXT(p,GL_GEOMETRY_INPUT_TYPE_EXT,GL_LINES);
	glProgramParameteriEXT(p,GL_GEOMETRY_OUTPUT_TYPE_EXT,GL_LINE_STRIP);

	int temp;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
	glProgramParameteriEXT(p,GL_GEOMETRY_VERTICES_OUT_EXT,temp);
	////////////////////////

	glLinkProgram(p);

	glUseProgram(p);

	// must define this after Use
	int pitchloc = glGetUniformLocation (p, "pitch");
	glUniform1f (pitchloc, 0.5);
}
	
int main(int argc, char **argv) 
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	mainwin = glutCreateWindow ("geometry shader");
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);

	viewer = ViewerNew (mainwin, 0,0,320,320, content);

	glewInit();
	if (glewIsSupported("GL_VERSION_2_1"))
		printf("Ready for OpenGL 2.1\n");
	else {
		printf("OpenGL 2.1 not supported\n");
		exit(1);
	}
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
		printf("Ready for GLSL - vertex, fragment, and geometry units\n");
	else {
		printf("Not totally ready :( \n");
		exit(1);
	}

	glClearColor(1.0,1.0,1.0,1.0);
	glShadeModel(GL_FLAT);
	init();

	glutMainLoop();

	return 0;
}
