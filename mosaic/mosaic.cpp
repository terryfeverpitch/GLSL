#include <gl/glew.h>
#include "gl/gluit.h"

#pragma comment (lib, "glew32.lib")

#include <stdio.h>

int width = 256;
int height = 256;

GLuint fbo;		// Our handle to the FBO
GLuint depthBuffer;	// Our handle to the depth render buffer
GLuint img;		// Our handle to a texture

void initFBO() {
   // Setup our FBO
   glGenFramebuffersEXT(1, &fbo);

   // bind for set up
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

   // Now setup a texture to render to
   glGenTextures(1, &img);
   glBindTexture(GL_TEXTURE_2D, img);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // And attach it to the FBO so we can render to it
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,          	GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img, 0);
   // Create the render buffer for depth	
   glGenRenderbuffersEXT(1, &depthBuffer);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT, 	width, height);

   // Attach the depth render buffer to FBO as it's depth attachment
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, 	GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);
	
   // Checking 
   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
      exit(1);	
   // Unbind the FBO for now
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);	
}

void ShutDown(GLvoid)
{
    glDeleteFramebuffersEXT(1, &fbo);
    glDeleteRenderbuffersEXT(1, &depthBuffer);
    glDeleteTextures(1,&img);
}


int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport (0,0,w,h);
	ViewerReshape(viewer);
}

GLuint p; // glsl program
float winxy[2] = {100.,100.};
float radius = 50.0;
GLuint centerloc;
GLuint radiusloc;

void content()
{
	// First we bind the FBO so we can render to it
	glUseProgram(0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutSolidTeapot(1.0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// Second pass
	glUseProgram(p); 
	glUniform2f (centerloc, winxy[0], winxy[1]);
	glUniform1f (radiusloc, radius);

	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_PROJECTION); glPushMatrix();
	glLoadIdentity(); gluOrtho2D (-1.2,1.2,-1.2,1.2);
	glMatrixMode (GL_MODELVIEW); glPushMatrix();
	glLoadIdentity();
	glEnable (GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, img);
	glBegin (GL_QUADS);
	glTexCoord2i(0,0); glVertex2i (-1,-1);
	glTexCoord2i(1,0); glVertex2i (1,-1);
	glTexCoord2i(1,1); glVertex2i (1,1);
	glTexCoord2i(0,1); glVertex2i (-1,1);
	glEnd();
	glDisable (GL_TEXTURE_2D);
	glMatrixMode (GL_PROJECTION); glPopMatrix();
	glMatrixMode (GL_MODELVIEW); glPopMatrix();
	
	glutSwapBuffers();
}


void init()
{
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);

	glewInit();

	initFBO();
	glClearColor (.4,.4,.4,1.);

	extern GLuint setShaders(char*,char*);
	p = setShaders ("mosaic.vert", "mosaic.frag");

	// in frag shader:
	// uniform vec2 center;	
	// uniform float radius;
	// uniform sampler2D tex;
	//uniform float gran; // granularity!

	glUseProgram(p);
	glUniform1i (glGetUniformLocation (p, "tex"), 0);

	GLuint granloc = glGetUniformLocation (p, "gran");
	glUniform1f (granloc, 40.0);

	centerloc = glGetUniformLocation (p, "center");
	glUniform2f (centerloc, winxy[0],winxy[1]);
	radiusloc = glGetUniformLocation (p, "radius");
	glUniform1f (radiusloc, radius);

}
void mouse (int button, int status, int x, int y)
{

	if (button == GLUT_RIGHT_BUTTON && status == GLUT_DOWN) {
		winxy[0] = x;
		winxy[1] = width - y;

		ViewerRedisplay(viewer);
	}
}

void keyboard (unsigned char key, int x, int y)
{	
	switch (key)
	{	
	case 'v':
		radius -= 1;
		break;
	case 'V':
		radius += 1;
	}
	
	ViewerRedisplay(viewer);
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize (width,height);
	mainwin = glutCreateWindow ("mosaic [right-click] [vV]");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	
	viewer = ViewerNew (mainwin, 0,0,width,height, content);
	ViewerAddClicker (viewer, mouse); 
	glutKeyboardFunc (keyboard);
	
	init();
	glutMainLoop();
}
