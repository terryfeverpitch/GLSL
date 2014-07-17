#include <gl/glew.h>
#include "gluit.h"
#pragma comment (lib, "glew32.lib")

// first pass render diffuse, normal, depth
//   no specular yet, (can be added)
//   (restricted to directional light?  unless we add a position map? )
//   can add SSAO effects...
 
// second pass
// render a screen space quad
// each fragment generates its due shades

// first, see if we can write diffuse to a texture ...


int mainwin, viewer;
int width, height;

void display(){}
void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}

GLuint mrtshader, defershader;

void scene()
{
	float red[] = {1., 0., 0., 1.};
	float yellow[] = {1., 1., 0., 1.};
	float green[] = {0., 1., 0., 1.};
	float blue[] = {0., 0., 1., 1.};
	float cyan[] = {0., 1., 1., 1.};
	float black[] = {0., 0., 0., 1.};
	float white[] = {1,1,1,1};

	glPushMatrix();
	glTranslatef (-4,0,0);
	glRotatef (30,0,1,0);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, red);
	glMaterialfv (GL_FRONT, GL_AMBIENT, black);
	glutSolidTeapot(1.0);
	glPopMatrix();
	
	glPushMatrix(); 
	glTranslatef (-2,0,0);
	glRotatef (30,0,1,0);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, yellow);
	glutSolidTeapot(1.0);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef (0,0,0);
	glRotatef (30,0,1,0);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, green);
	glutSolidTeapot(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef (2,0,0);
	glRotatef (30,0,1,0);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, cyan);
	glutSolidTeapot(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef (4,0,0);
	glRotatef (30,0,1,0);
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	glutSolidTeapot(1.0);
	glPopMatrix();
	
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glBegin (GL_QUADS);
	glNormal3f (0,1,0);
	glVertex3i (-7,-1,7);
	glVertex3i ( 7,-1,7);
	glVertex3i ( 7,-1,-7);
	glVertex3i (-7,-1,-7);
	glEnd();
}

float theta;
int toggle;
float lightPosition[4] = {5,5,5,0};
#define MULT_LIGHT_PROJECTION() glFrustum (-2,2,-2,2,2,8)
#define MULT_LIGHT_MODELVIEW()   gluLookAt(lightPosition[0],lightPosition[1],lightPosition[2],\
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f)

void content()
{
	glViewport (0,0,400,400);
	// Render-to-texture pass
	extern GLuint fbo;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);	

	glMatrixMode (GL_PROJECTION); glLoadIdentity();
	gluPerspective (65, 1, .5,40);  // a narrower depth range
	glMatrixMode (GL_MODELVIEW);

	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glUseProgram (mrtshader);
	scene();
	glUseProgram(0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// deferred shade pass
//	glViewport (0,0,800,400);
	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	glPushMatrix();
//	glRotatef (theta, 0,1,0);
//	float pos[] = {.1,.1,.1,0};
	glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
	glPopMatrix();

extern void SHADOW_MAP_PASS();
SHADOW_MAP_PASS();
	
	glViewport (0,0,400,400);

	glUseProgram (defershader);
	glUniform1i (glGetUniformLocation (defershader, "img_diffuse"), 0);
	glUniform1i (glGetUniformLocation (defershader, "img_normal"), 1);
	glUniform1i (glGetUniformLocation (defershader, "img_depth"), 2);
	glUniform1i (glGetUniformLocation (defershader, "shadowMap"), 3);
	
	extern GLuint img_diffuse, img_depth, img_normal;

	glActiveTexture (GL_TEXTURE0); glBindTexture (GL_TEXTURE_2D, img_diffuse);
	glActiveTexture (GL_TEXTURE1); glBindTexture (GL_TEXTURE_2D, img_normal);
	glActiveTexture (GL_TEXTURE2); glBindTexture (GL_TEXTURE_2D, img_depth);

// SHADOW PART
	glActiveTexture (GL_TEXTURE3);  // use the last texture unit

	glMatrixMode (GL_TEXTURE);	glLoadIdentity();
	static float b[16] = {.5, 0,0,0, 0,.5,0,0, 0,0,.5,0, .5,.5,.5,1};
	glMultMatrixf (b);  // scale and bias
	MULT_LIGHT_PROJECTION();
	MULT_LIGHT_MODELVIEW();

	extern void apply_modelview_inverse();
	apply_modelview_inverse ();  // camera view inverse 

	glMatrixMode (GL_MODELVIEW);

//////////////////////////////////
	
	BEGIN_2D_OVERLAY (10,10);
	glBegin (GL_QUADS);  // full-screen rendering
	glTexCoord2f (0,0); glVertex2i (0,0);
	glTexCoord2f (1,0); glVertex2i (10,0);
	glTexCoord2f (1,1); glVertex2i (10,10);
	glTexCoord2f (0,1); glVertex2i (0,10);
	glEnd();
	END_2D_OVERLAY();

	glUseProgram(0);

	// regular rendering
	// for comparison
	glViewport (400,0,400,400);
	scene();
	
	BEGIN_2D_OVERLAY(10,10);
		drawstr (1,1, "no deferred");
	END_2D_OVERLAY();

	glutSwapBuffers();
}


void init()
{
	glewInit(); 

	glClearColor (1,1,1,1);

	extern void initFBO();
	initFBO();
extern void init_shadowmap();
init_shadowmap();

	glEnable (GL_DEPTH_TEST);	
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);

	float pos[] = {1,1,1,0};
	glLightfv (GL_LIGHT0, GL_POSITION, pos);

	extern GLuint setShaders (char*, char*);
	mrtshader = setShaders ("mrt.vert", "mrt.frag");
	defershader = setShaders ("defershade.vert", "defershade.frag");
}

void timer (int dummy)
{
	glutTimerFunc (100, timer, 0);
	theta += 2;
	glutPostRedisplay();
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (800,400);
	mainwin = glutCreateWindow ("deferred shading [t]");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew (mainwin, 0,0,800,400, content);
	init();
	ViewerExtent (viewer, 10.0);
	glutTimerFunc (10, timer, 0);

	glutMainLoop();
}
