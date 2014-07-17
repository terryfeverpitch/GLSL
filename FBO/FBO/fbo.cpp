#include <gl/glew.h>
#include "gl/gluit.h"
#pragma comment (lib, "glew32.lib")

int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}

GLuint fbo;		// Our handle to the FBO
GLuint depthBuffer;	// Our handle to the depth render buffer
GLuint img;		// Our handle to a texture


void content()
{
	// RTT (pass)
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable (GL_TEXTURE_2D);
	glEnable (GL_LIGHTING);
	glViewport(0,0,400,400);

	glClearColor(1.0f, 0.0f, 0.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
static float ang;

	glPushMatrix();
	glRotatef (ang, 0,1,0);	
	glutSolidTeapot (1.0);
	glPopMatrix();
ang += 3; 
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//// end of RTT

	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	glEnable (GL_TEXTURE_2D);	
	glBindTexture (GL_TEXTURE_2D, img);

	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin (GL_QUADS);
	glNormal3f (0,0,1);
	glTexCoord2f (0,0); glVertex2i (-1,0);
	glTexCoord2f (1,0); glVertex2i (1,0);
	glTexCoord2f (1,1); glVertex2i (1,2);
	glTexCoord2f (0,1); glVertex2i (-1,2);
	glEnd();

	glutSwapBuffers();
}

void initFBO() {
   // Setup our FBO
   glGenFramebuffersEXT(1, &fbo);

   // bind for set up
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

   // Now setup a texture to render to
   glGenTextures(1, &img);
   glBindTexture(GL_TEXTURE_2D, img);
   
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  400,400, 0, GL_RGBA,      	GL_UNSIGNED_BYTE, NULL);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// And attach it to the FBO so we can render to it
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img, 0);

   // Create the render buffer for depth	
   glGenRenderbuffersEXT(1, &depthBuffer);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT, 	400,400);

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

void init()
{
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glClearColor (.4,.4,.4,1);
	initFBO();
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("tst");

	glewInit();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew (mainwin, 0,0,400,400, content);
	glutIdleFunc (content);

	init();
	glutMainLoop();
}
