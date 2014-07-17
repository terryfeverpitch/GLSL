#include <gl/glew.h>
#include <gl/glpng.h>
#include "gl/gluit.h"
#include "svl/svl.h"
#pragma comment (lib, "glew32.lib")
int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}

GLuint fbo,fbo2;		// Our handle to the FBO
GLuint depthBuffer,depthBuffer2;	// Our handle to the depth render buffer
GLuint img,img2;		// Our handle to a texture
int width=400, height=400;
double theta;
GLuint tex;
GLMmodel *al;
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
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img, 0);
   // Create the render buffer for depth	
   glGenRenderbuffersEXT(1, &depthBuffer);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT, width, height);

   // Attach the depth render buffer to FBO as it's depth attachment
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);
	
   // Checking 
   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
      exit(1);	
   // Unbind the FBO for now
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);	
}
void initFBO2() {
   // Setup our FBO
   glGenFramebuffersEXT(1, &fbo2);

   // bind for set up
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo2);

   // Now setup a texture to render to
   glGenTextures(1, &img2);
   glBindTexture(GL_TEXTURE_2D, img2);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // And attach it to the FBO so we can render to it
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img2, 0);
   // Create the render buffer for depth	
   glGenRenderbuffersEXT(1, &depthBuffer2);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer2);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT, width, height);

   // Attach the depth render buffer to FBO as it's depth attachment
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer2);
	
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
void ShutDown2(GLvoid)
{
    glDeleteFramebuffersEXT(1, &fbo2);
    glDeleteRenderbuffersEXT(1, &depthBuffer2);
    glDeleteTextures(1,&img2);
}
GLuint prog,prog2;
void floor()
{

	glPushAttrib (GL_ALL_ATTRIB_BITS);
	glBindTexture(GL_TEXTURE_2D, tex);
	glDisable (GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBegin (GL_QUADS);
	glTexCoord2f (0,0); glVertex3i (-10,0,10); 
	glTexCoord2f (3,0); glVertex3i (10,0,10);
	glTexCoord2f (3,3); glVertex3i (10,0,-10); 
	glTexCoord2f (0,3); glVertex3i (-10,0,-10);
	glEnd();
	glPopAttrib();

}
void scene()
{
	floor();
	glUseProgram(0);
	glPushAttrib (GL_ALL_ATTRIB_BITS);
	glEnable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	float red[]={1,0,0,1};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, red);
	glPushMatrix();
	glTranslatef(-2,0,-3);
	glRotatef(-90,1,0,0);
	glutSolidCone(1.0,2.0,20,20);
	glPopMatrix();

	float yellow[]={1,1,0,1};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, yellow);
	glPushMatrix();
	glTranslatef(2,0,3);
	glRotatef(-90,1,0,0);
	glutSolidCone(1.0,2.0,20,20);
	glPopMatrix();
	glPopAttrib();
}

void content()
{
	// 開始畫fbo1
	glUseProgram(0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// 畫背景scene
	// fbo1 結束
	scene();
	// 開始畫fbo2，並且使用gery filter
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo2);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE); //關掉depthMask
	
	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
	glUseProgram(prog);
	glBindTexture(GL_TEXTURE_2D, img);	// 把fbo1 畫到 fbo2
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(-1,-1);
	glTexCoord2f(1, 0); glVertex2i( 1,-1);
	glTexCoord2f(1, 1); glVertex2i( 1, 1);
	glTexCoord2f(0, 1); glVertex2i(-1, 1);
	glEnd();
	
	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW);  glPopMatrix();

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	scene(); // 讓畫AL時，也可以知道scene的深度資訊

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // 畫AL前 再把colorMask 打開
	// 畫AL()，但是畫AL之前要先關掉gery filter
	glUseProgram(0);
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPushMatrix();
	float r = 1.5;
	glTranslatef (r*cos(theta*vl_pi/180), 0, -r*sin(theta*vl_pi/180));
	glRotatef (theta+180, 0,1,0);
	glTranslatef (0,1,0);
	glmDraw (al, GLM_MATERIAL|GLM_SMOOTH);

	glPopMatrix();
	glPopAttrib();
	// 到這邊為止 fbo1 已經畫到 fbo2 上了，所以fbo2上已經有colored AL and gery background
	
	// 開始畫main windows，並且使用mosaic filter
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(prog2);

	double winx, winy, winz;
	
	double mv[16], proj[16];
	int viewport[4];
	glGetDoublev (GL_MODELVIEW_MATRIX, mv);
	glGetDoublev (GL_PROJECTION_MATRIX, proj);
	glGetIntegerv (GL_VIEWPORT, viewport);
	gluProject (0,0.9,0, mv, proj, viewport, &winx, &winy, &winz);

	glUniform2f (glGetUniformLocation (prog, "center"), winx,winy);
	glUniform1f (glGetUniformLocation (prog, "radius"), 25);

	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, img2); // 要注意是要把 fbo2 畫在 main windows 上，而不是 fbo1 畫在 main windows 上
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(-1,-1);
	glTexCoord2f(1, 0); glVertex2i( 1,-1);
	glTexCoord2f(1, 1); glVertex2i( 1, 1);
	glTexCoord2f(0, 1); glVertex2i(-1, 1);
	glEnd();
	
	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW);  glPopMatrix();
	// main windows 結束
	
	glutSwapBuffers();
}
void init()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glClearColor(0.4, 0.4, 0.4, 1.0);
	
	initFBO();
	initFBO2();

	al = glmReadOBJ ("al.obj");
	glmUnitize (al);
	glmFacetNormals (al);
	glmVertexNormals (al, 90.0);
	
	tex = pngBind("checkboard.png", PNG_NOMIPMAP, PNG_ALPHA, NULL, GL_REPEAT, GL_NEAREST, GL_NEAREST);

	extern GLuint setShaders (char*, char*);
	prog = setShaders ("greyFilter.vert", "greyFilter.frag");
	prog2 = setShaders ("mosaicFilter.vert", "mosaicFilter.frag");
	
	glUseProgram (prog2);
	glUniform1i (glGetUniformLocation (prog, "tex"), 0);
	glUniform1f (glGetUniformLocation (prog, "gran"), 80);
	glUseProgram (0);

}
int turn;
void timer (int dummy)
{
	glutTimerFunc (10, timer, 0);
	glutPostRedisplay();
	if (turn) theta += 2;
}
void keyboard (unsigned char key, int x, int y)
{
	if (key == ' ')
		turn ^= 1;
	glutPostRedisplay();
}
void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("tst");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	
	viewer = ViewerNew (mainwin, 0,0,400,400, content);
	glutTimerFunc (10, timer, 0);
	glutKeyboardFunc (keyboard);

	init();
	glutMainLoop();
}
