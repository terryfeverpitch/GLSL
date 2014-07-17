#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <gl/glew.h>
#include "gluit.h"
#include "gl/glpng.h"

#pragma comment (lib, "glew32.lib")


GLMmodel *pmodel;
GLuint sphmapid;

static void 
InitGraphics(void)
{
	pngSetStandardOrientation(1);
	sphmapid = pngBind("sphere_shade3.png", PNG_BUILDMIPMAPS, PNG_SOLID, NULL, GL_CLAMP, 
				GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	
	pmodel = glmReadOBJ ("al.obj");
	glmUnitize (pmodel);
	glmFacetNormals (pmodel);
	glmVertexNormals (pmodel, 90.0);
}

void axes()
{
	glPushAttrib (GL_ENABLE_BIT|GL_CURRENT_BIT|GL_LINE_BIT);
	glDisable (GL_TEXTURE_2D);
	glLineWidth (5.0);
	glBegin (GL_LINES);
	glColor3ub (255,0,0);
	glVertex2i (0,0), glVertex3i (1,0,0);
	glColor3ub (0,255,0);
	glVertex2i (0,0), glVertex3i (0,1,0);
	glColor3ub (0,0,255);
	glVertex2i (0,0), glVertex3i (0,0,1);
	glEnd();
	glPopAttrib();
}

void grid(void)
{
	glPushAttrib (GL_ENABLE_BIT);
	glDisable (GL_TEXTURE_2D);
	glColor3ub (255,255,255);
	glBegin (GL_LINES);
	for (int i = -10; i <= 10; i++) {
		glVertex3i (-10,0,i); glVertex3i (10,0,i);
		glVertex3i (i,0,-10); glVertex3i (i,0,10);
	}
	glEnd();
	glPopAttrib();
}

GLuint program;

static void 
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glUseProgram(0);
//	grid();
//	axes();
	

	glUseProgram (program);
	glBindTexture (GL_TEXTURE_2D, sphmapid);
	glmDraw (pmodel, GLM_MATERIAL|GLM_SMOOTH);

	glutSwapBuffers();
}

int mainwin, viewer;
static void
keyboard(unsigned char c, int x, int y)
{

	switch (c) {
	case 27:
		exit(0);
		break;
	default:
		return;
	}
	ViewerRedisplay(viewer);
}

void _display(void){}
void _reshape(int w, int h) 
{
	glViewport (0,0,w,h);
	ViewerReshape (viewer);
}


void initShader ()
{
	extern GLuint setShaders(char*,char*);

	program = setShaders ("spheremap.vert", "spheremap.frag");
	glUseProgram (program);
	int sphmaploc = glGetUniformLocation (program, "sphmap");
	glUniform1i (sphmaploc, 0);

	glUseProgram (0);
}

int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize(400,400);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  mainwin = glutCreateWindow("spheremap");

  glewInit();

  glutDisplayFunc(_display);
  glutReshapeFunc (_reshape);

  viewer = ViewerNew (mainwin, 0,0,400,400, display);
  glutKeyboardFunc(keyboard);
 
  glEnable(GL_DEPTH_TEST);
  InitGraphics();
  initShader();
  glClearColor (.4,.4,.4,1);
  
  glutMainLoop();
  return 0;
}
