#include <gl/glew.h>
#include "gl/gluit.h"
#include "gl/glpng.h"
//#include "helper.h"

#pragma comment (lib, "glew32.lib")

#include <stdio.h>
extern GLuint fbo, depthBuffer, img, img2;
extern void initfbo();
int width = 256;
int height = 256;

int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport (0,0,w,h);
	ViewerReshape(viewer);
}

GLuint p_mrt, p_2sides; // glsl programs

double angle;

void content()
{
	// First pass, render to two textures
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glUseProgram(p_mrt);
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef (angle, 0,1,0);
	glutWireTeapot(1.0);
	glPopMatrix();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glUseProgram(0);
	glBegin (GL_LINE_LOOP);
	glVertex2i (-2,-2); glVertex2i (2,-2); glVertex2i (2,2); glVertex2i (-2,2);
	glEnd();

	// second pass, render to both side textures
	glUseProgram (p_2sides);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, img);
	glActiveTexture (GL_TEXTURE1);
	glBindTexture (GL_TEXTURE_2D, img2);

	glEnable (GL_POLYGON_OFFSET_FILL);
	glPolygonOffset (0.2, 0.2);
	glBegin (GL_QUADS);
	glMultiTexCoord2f (GL_TEXTURE0, 0.0, 0.0); glVertex2i (-2,-2);
	glMultiTexCoord2f (GL_TEXTURE0, 1.0, 0.0); glVertex2i ( 2,-2);
	glMultiTexCoord2f (GL_TEXTURE0, 1.0, 1.0); glVertex2i ( 2,2);
	glMultiTexCoord2f (GL_TEXTURE0, 0.0, 1.0); glVertex2i (-2,2);
	glEnd();
	glDisable (GL_POLYGON_OFFSET_FILL);


	glutSwapBuffers();
}


void init()
{
	glewInit();

	glEnable (GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.2f, 0.5f);
	glClearDepth(1.0f);					
	glEnable(GL_DEPTH_TEST);			
	glDepthFunc(GL_LEQUAL);				

	initfbo();
	glClearColor (.4,.4,.4,1.);

	////////////////////////////////////
	// MRT shader
	extern GLuint setShaders(char*,char*);
	p_mrt = setShaders ("mrt.vert", "mrt.frag");

	////////////////////////////////////
	// two-side texture shader

	p_2sides = setShaders ("twoside.vert", "twoside.frag");
	glUseProgram(p_2sides);
	glUniform1i (glGetUniformLocation (p_2sides, "fronttex"), 0);
	glUniform1i (glGetUniformLocation (p_2sides, "backtex"), 1);
	glUseProgram(0);
}

void timer (int dummy)
{
	angle += 2;

	glutTimerFunc (10, timer, 0);
	ViewerRedisplay (viewer);
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize (width,height);
	mainwin = glutCreateWindow ("MRT");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
		
	// both 4,4 on my old sony vaio S56TP
	int maxbuffers;
	glGetIntegerv (GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);
	printf ("max color attachments: %d\n", maxbuffers);
	glGetIntegerv (GL_MAX_DRAW_BUFFERS, &maxbuffers);
	printf ("max draw buffers: %d\n", maxbuffers);

	viewer = ViewerNew (mainwin, 0,0,width,height, content);
	init();
	glutTimerFunc (10, timer, 0);
	
	glutMainLoop();
}
