#include "gl/glew.h"
#include "gl/gluit.h"
#include "gl/glpng.h"
#include "svl/svl.h"

#pragma comment (lib, "glew32.lib")

int mainwin, viewer;
int gw = 400, gh = 400;
float radius = 2.5;
float theta = 0;

GLuint greyFilter, mosaicFilter;
GLuint checkboard;
GLMmodel *m1;

bool USE_GREY_FILTER   = true;
bool USE_MOSAIC_FILTER = true;
bool AL_ANIMATE		   = false;

void display()
{
	ViewerRedisplay(viewer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GLuint fbo1;			// Our handle to the FBO
GLuint fbo2;																					
GLuint depthBuffer1;	// Our handle to the depth render buffer 
GLuint depthBuffer2;	                                                         
GLuint img1;			// Our handle to a texture	
GLuint img2;			// the second FBO texture																		

int width, height;																											

void initFBO(GLuint& fbo, GLuint& depthBuffer, GLuint& img) {																											
	// Setup our FBO
	width = gw; height = gh;
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
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img, 0);
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reshape(int w, int h)
{
	gw = w; gh = h;
	
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
	initFBO(fbo1, depthBuffer1, img1);
	initFBO(fbo2, depthBuffer2, img2);
}

void floor()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, checkboard);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2i (0,0); glVertex2i (-5,-5);
	glTexCoord2i (1,0); glVertex2i ( 5,-5);
	glTexCoord2i (1,1); glVertex2i ( 5, 5);
	glTexCoord2i (0,1); glVertex2i (-5, 5);
	glEnd();
	glPopMatrix();
	glPopAttrib();
}

void cone(float x, float y, float z, float color[])
{
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(0.8, 2.0, 20, 20);
	glPopMatrix();
}

void scene()
{
	floor();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	float red[]   = {1, 0, 0};
	float green[] = {0, 1, 0};
	float blue[]  = {0, 0, 1};
	float black[] = {0, 0, 0};

	cone( 3, 0, 3, red);	
	cone(-3, 0, 3, green);
	cone(-3, 0,-3, blue);
	cone( 3, 0,-3, black);
	glPopAttrib();
}

void AL(float theta, float radius)
{
	glUseProgram(0);
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glRotatef(theta, 0, 1, 0);
	glTranslatef(-radius, 1, 0);
	glmDraw(m1, GLM_SMOOTH | GLM_MATERIAL);
	glPopAttrib();
}

void overlay()
{	
	glUseProgram(0);
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	BEGIN_2D_OVERLAY (gw, gh);
	
	if(USE_GREY_FILTER) {
		glColor3f(1, 0, 0);
		drawstr(10, gh - 20, "GREY_FILTER = ON");
	}
	else {
		glColor3f(1, 1, 0);
		drawstr(10, gh - 20, "GREY_FILTER = OFF");
	}

	if(USE_MOSAIC_FILTER) {
		glColor3f(1, 0, 0);
		drawstr(10, gh - 50, "MOSAIC_FILTER = ON");
	}
	else {
		glColor3f(1, 1, 0);
		drawstr(10, gh - 50, "MOSAIC_FILTER = OFF");
	}

	glColor3f(1, 1, 0);
	drawstr(10, 10, "Press c/C to enable/disable program to use grey filter.");
	drawstr(10, 40, "Press m/M to enable/disable program to use mosaic filter.");
	drawstr(10, 70, "Press Space to enable/disable AL to animate.");
	END_2D_OVERLAY();
	glPopAttrib();
}

void content()
{
	// start render to tex1 pass
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // clear FBO
	glUseProgram(0);
	scene();
	// end of render to tex1 pass

	// start render tex1 to tex2 pass
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo2);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE); // disable writting to depth buffer

	if(USE_GREY_FILTER)
		glUseProgram(greyFilter); // use grey filter
	else
		glUseProgram(0);
	
	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, img1);
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
	scene(); // draw again with only depth info

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glPushMatrix();
	AL(theta, radius);
	// compute head window coordinate
	// Al's head is about (0,1,0) world coordinate
	// NDC = proj ( [PJ][MV](head) )
	double winx, winy, winz;
	double lower_x, lower_y, lower_z;
	double origin_x, origin_y, origin_z;
	double front_x, front_y, front_z;
	
	double mv[16], proj[16];
	int viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, mv);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluProject(0, 1.0, 0, mv, proj, viewport, &winx, &winy, &winz);
	gluProject(0, 0.3, 0, mv, proj, viewport, &lower_x, &lower_y, &lower_z);

	gluProject(0, 0, 0, mv, proj, viewport, &origin_x, &origin_y, &origin_z);
	gluProject(1, 0, 0, mv, proj, viewport, &front_x, &front_y, &front_z);
	Vec3 v = Vec3(front_x - origin_x, front_y - origin_y, front_z - origin_z);

	glMatrixMode (GL_PROJECTION); glPopMatrix();
	glMatrixMode (GL_MODELVIEW); glPopMatrix();

	glPopMatrix();
	// end of render tex1 to tex2 pass
	if(USE_MOSAIC_FILTER) {
		glUseProgram(mosaicFilter);
		glUniform1f(glGetUniformLocation(mosaicFilter, "gran"), 80.0);
		float head[2] = {winx, (winy + lower_y) / 2.0};
		glUniform2fv(glGetUniformLocation(mosaicFilter, "center"), 1, head);
		float radius = (winy - lower_y) / 2.0;
		glUniform1f(glGetUniformLocation(mosaicFilter, "radius"), radius);
		//cout << v << endl;
	}
	else
		glUseProgram(0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	// use FBOs for texture
	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, img2);
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
	// end of using FBOs for texture*/

	overlay();
	
	glutSwapBuffers();
}

void init()
{
	glewInit();
	glEnable (GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glClearColor(.4, .4, .4, 1);
	initFBO(fbo1, depthBuffer1, img1);
	initFBO(fbo2, depthBuffer2, img2);

	checkboard = pngBind("checkboard.png", PNG_NOMIPMAP, PNG_ALPHA, NULL, GL_REPEAT, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, checkboard);
	glEnable(GL_TEXTURE_2D);

	extern GLuint setShaders(char* vert_file, char* frag_file);
	greyFilter = setShaders("greyFilter.vert", "greyFilter.frag");
	mosaicFilter = setShaders("mosaicFilter.vert", "mosaicFilter.frag");

	// load OBJ model
	m1 = glmReadOBJ ("al.obj");
	glmUnitize (m1);
	glmFacetNormals (m1);
	glmVertexNormals (m1, 90);
}

void keyboard(unsigned char key, int x, int y)
{
	if(key == 'c' || key == 'C')
		USE_GREY_FILTER ^= 1;
	else if(key == 'm' || key == 'M')
		USE_MOSAIC_FILTER ^= 1;
	else if(key == ' ')
		AL_ANIMATE ^= 1;

	glutPostRedisplay();
}

void timer(int dummy)
{
	if(AL_ANIMATE)
		theta += 1.5;

	glutTimerFunc(10, timer, 0);
	glutPostRedisplay();
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("Part 3 Integration");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew (mainwin, 0,0,400,400, content);
	init();
	glutKeyboardFunc(keyboard);
	glutTimerFunc(10, timer, 0);
	glutMainLoop();
}