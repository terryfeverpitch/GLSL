//
// references:
//    http://homepages.inf.ed.ac.uk/rbf/HIPR2/featops.htm
//


#define MAIN "filter"

#define IMAGE_WIDTH  256
#define IMAGE_HEIGHT 256

#include <GL/glew.h>

#include <glut.h>
#include "gl/glpng.h"

void display (void)
{
	glBegin (GL_QUADS);
		glTexCoord2i (0,0); glVertex2i (-1,-1);
		glTexCoord2i (1,0); glVertex2i (1,-1);
		glTexCoord2i (1,1); glVertex2i (1,1);
		glTexCoord2i (0,1); glVertex2i (-1,1);
	glEnd();

	glutSwapBuffers();
}

void reshape (int w, int h)
{
	glViewport (0,0,w,h);
	glMatrixMode (GL_PROJECTION); glLoadIdentity(); gluOrtho2D (-1,1,-1,1);
	glMatrixMode (GL_MODELVIEW); glLoadIdentity();
}

void init (void)
{
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
	p = setShaders("filter.vert", "filter0.frag");

	glUseProgram (p);

	glUniform1i (glGetUniformLocation (p, "colorMap"), 0);
	
	glUniform1f (glGetUniformLocation (p, "width"), (float)IMAGE_WIDTH);
	glUniform1f (glGetUniformLocation (p, "height"), (float)IMAGE_HEIGHT);

	pngSetStandardOrientation (1);
	GLuint id = pngBind("olympic.png", PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_NEAREST, GL_NEAREST);
}


void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowSize (IMAGE_WIDTH, IMAGE_HEIGHT);
	glutCreateWindow (MAIN);
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);

	init();

	glutMainLoop();
}

