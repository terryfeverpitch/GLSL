#include <gl/glew.h>
#include "gl/gluit.h"
#pragma comment (lib, "glew32.lib")

#include <stdio.h>

int mainwin, viewer;
GLuint program;

/////////////////////////////////////////////////////////////
// scene related API
extern void DrawScene ();
extern void initScene(int);  // pass shader program 
                             // (for setting scene-related uniform variables)
extern void scene_timer();
extern void scene_keys (unsigned char);
extern void get_scene_light_pos (float*);

// light positions
static float lightPosition[4];
float _fovy_ = 95.0;

// not needed for GLSL
// Texture object for shadow texture
// GLuint shadowMapTexture;

// viewport size
int windowWidth = 512;
int windowHeight = 512;
int gw = windowWidth, gh = windowHeight;
// shadow map size
GLuint fboId, depthTextureId;
int SHADOW_MAP_RATIO = 1;
//int shadowMapWidth = windowWidth*shadowMapRatio;
//int shadowMapHeight = windowHeight*shadowMapRatio;

int use_pcf = 1;
int usePCFLoc;

void display(void){}

void initShadowmapShaders(void)
{
	extern GLuint setShaders (char*,char*);

	program = setShaders ("shadowmap.vert", "shadowmap.frag");
	glUseProgram(program);
	int shadowMaploc = glGetUniformLocation (program, "shadowMap");
	glUniform1i(shadowMaploc, 3);   // set shadowmap to texture unit 3

	int xPixelOffsetLoc = glGetUniformLocation (program, "xPixelOffset");
	int yPixelOffsetLoc = glGetUniformLocation (program, "yPixelOffset");
	glUniform1f (xPixelOffsetLoc, 1.0/windowWidth/SHADOW_MAP_RATIO);   
	glUniform1f (yPixelOffsetLoc, 1.0/windowHeight/SHADOW_MAP_RATIO);   
	
	usePCFLoc = glGetUniformLocation(program, "use_pcf");
	glUniform1i(usePCFLoc, use_pcf);   
	glUniform1i(glGetUniformLocation(program, "antiMode"), 1); 
	
	glUseProgram(0);   // disable shader for now
}

void generateShadowFBO()
{
	int shadowMapWidth = gw * SHADOW_MAP_RATIO;
	int shadowMapHeight = gh * SHADOW_MAP_RATIO;
	
	//GLfloat borderColor[4] = {0,0,0,0};
	glActiveTexture(GL_TEXTURE3);	
	// Try to use a texture depth component
	glGenTextures(1, &depthTextureId);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);
	
	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF. Using GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// Remove artefact on the edges of the shadowmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	// This is to allow usage of shadow2DProj function in the shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); 
	
	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// create a framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
	
	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	// attach the texture to FBO depth attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depthTextureId, 0);
	
	// check FBO status
	GLenum FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");
	
	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


bool Init(void)
{
	glewInit();
	if (! GLEW_ARB_depth_texture || ! GLEW_ARB_shadow) {
		printf("I require ARB_depth_texture and ARB_shadow extensions\n");
		return false;
	}

	initShadowmapShaders();
	
	// default settings; can be omitted
    //
	// lighting is implied in shaders
	// glShadeModel(GL_SMOOTH);
	// glClearDepth(1.0f);
	// glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	//We use glScale when drawing the scene
	// glEnable(GL_NORMALIZE);

	//	glEnable(GL_CULL_FACE);  ... replaced by polygon offset


	// Create the shadow map texture (to texture unit 3)
	// and attach to FBO
	generateShadowFBO();
	
	initScene(program);

	return true;
}

void apply_inverse (void)
{
	// note that modelview has been restored to camera view AT THIS POINT!!
	float m[16], mv[16];
	
	glGetFloatv (GL_MODELVIEW_MATRIX, m);
	mv[12] = -(m[0]*m[12] + m[1]*m[13] + m[2]*m[14]);
	mv[13] = -(m[4]*m[12] + m[5]*m[13] + m[6]*m[14]);
	mv[14] = -(m[8]*m[ 12] + m[9]*m[13] + m[10]*m[14]);
    mv[15] = 1.0;

	mv[0] = m[0]; mv[4] = m[1]; mv[8] = m[2]; 
	mv[1] = m[4]; mv[5] = m[5]; mv[9] = m[6];
	mv[2] = m[8]; mv[6] = m[9]; mv[10]= m[10];
	mv[3] = 0.0;  mv[7] = 0.0;  mv[11] = 0.0;

    glMultMatrixf (mv);
}

void SetWorldLight ()
{
	get_scene_light_pos (lightPosition);
	glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
}

void ShowLight()
{
	// show light ...
	glUseProgram (0);
	glDisable(GL_LIGHTING);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix(); 
	glTranslatef (lightPosition[0],lightPosition[1],lightPosition[2]);
	glColor3ub (255,255,0); glutSolidSphere (0.1,12,12);
	glPopMatrix();
}

void MULT_LIGHT_PROJECTION()
{
	gluPerspective(_fovy_, 1.0f, 2.0f, 8.0f);  // light projection
}

void MULT_LIGHT_MODELVIEW()
{
	gluLookAt(lightPosition[0],lightPosition[1],lightPosition[2],
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);   // light view
}

void SHADOW_MAP_PASS()
{
	//First step: Render from the light POV to a FBO, story depth values only
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);	//Rendering offscreen

	//////////////////////////////////////////////
	// First pass - from light's point of view
	glClear(GL_DEPTH_BUFFER_BIT);  // no output to colorbuffer

	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
	glViewport(0,0, windowWidth * SHADOW_MAP_RATIO,windowHeight* SHADOW_MAP_RATIO);
	
	// Use viewport the same size as the shadow map
	// in NON-FBO implementation, the shadowMap cannot be larger than the viewport size	
	// glViewport(0, 0, shadowMapSize, shadowMapSize);

	glMatrixMode(GL_PROJECTION); glPushMatrix();   glLoadIdentity();// save current projection
	//glLoadIdentity(); gluPerspective(_fovy_, 1.0f, 2.0f, 8.0f);  // light projection
	MULT_LIGHT_PROJECTION();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();  glLoadIdentity(); // viewer: save current modelview (containing Viewing of camera)
	MULT_LIGHT_MODELVIEW();
	//glLoadIdentity();
	//gluLookAt(lightPosition[0],lightPosition[1],lightPosition[2],
	//		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);   // light view


	//Disable color writes, and use flat shading for speed
	glColorMask(0, 0, 0, 0);
	glShadeModel(GL_FLAT);

	// disable texturingin the first pass (if any)
	// disable lighting too
	glPushAttrib (GL_ENABLE_BIT);
	glDisable (GL_LIGHTING);
	glDisable (GL_TEXTURE_2D);
	//Draw back faces into the shadow map
	// IMPORTANT: to remove moire pattern (from z-fighting)
	glPolygonOffset (8.0, 4.0);
	glEnable (GL_POLYGON_OFFSET_FILL);

	// Draw the scene (fixed function)
	glUseProgram(0);
	DrawScene();
//	glDisable (GL_POLYGON_OFFSET_FILL);
	glPopAttrib();

	glPopMatrix();  // restore Modelview to Vcamera
	glMatrixMode(GL_PROJECTION); glPopMatrix();  // restore current projection

	//Read the depth buffer into the shadow map texture
//	glActiveTexture (GL_TEXTURE3);
//	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
//	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);

	//restore states
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
}

void RENDERING_PASS ()
{
	// Now rendering from the camera POV, using the FBO to generate shadows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

	////////////////////////////////////////////////////////
	//2nd pass - render both shadowed and lit part
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// textureMatrix=biasMatrix*lightProjectionMatrix*lightViewMatrix*cameraViewInverse
	// store the coordinate conversion matrix to texture matrix[3]
	// to be used in shader
	glActiveTexture (GL_TEXTURE3);  // use the last texture unit

	glMatrixMode (GL_TEXTURE);	glLoadIdentity();
	static float b[16] = {.5, 0,0,0, 0,.5,0,0, 0,0,.5,0, .5,.5,.5,1};
	glMultMatrixf (b);  // scale and bias
	MULT_LIGHT_PROJECTION();
	MULT_LIGHT_MODELVIEW();
	// gluPerspective(_fovy_, 1.0f, 2.0f, 8.0f);  // light projection
	// gluLookAt(lightPosition[0],lightPosition[1],lightPosition[2],
	//		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);   // light view

	apply_inverse ();  // camera view inverse

	glMatrixMode (GL_MODELVIEW);

	//Bind & enable shadow map texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);
//	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
//	glEnable(GL_TEXTURE_2D);  // IMPLIED by shader

	// three important settings to use depth texture (see notes)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

//	SetWorldLight(lightPosition);
	glUseProgram (program);
	DrawScene();

	//Disable textures
	//	glDisable(GL_TEXTURE_2D);

}

void Display(void)
{
	SetWorldLight ();

	SHADOW_MAP_PASS();

	RENDERING_PASS();
	
	// FOR visual verification
	ShowLight();

	BEGIN_2D_OVERLAY (10,10);
	if (use_pcf) 
		drawstr (1,1, "Use PCF");
	else
		drawstr (1,1, "NO PCF");
	END_2D_OVERLAY();
	
	glutSwapBuffers();
}

void Reshape (int w, int h)
{
	//Save new window size
	gw =w; gh = h;
	windowWidth=w, windowHeight=h;

	glViewport (0,0,w,h);
	ViewerReshape(viewer);

	generateShadowFBO();
}

int antiMode = 1;

//Called when a key is pressed
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27: // escape
		exit(0);
		break;
	case 'p':
		use_pcf ^= 1;
		glUseProgram(program);  // this is a MUST!!!
		glUniform1i (usePCFLoc, use_pcf);   // program must be activated for a uniform variable to be set
		ViewerRedisplay(viewer);
		break;
	case 'm': case 'M':
		antiMode ^= 1;
		glUseProgram(program);  // this is a MUST!!!
		glUniform1i(glGetUniformLocation(program, "antiMode"), antiMode); 
		ViewerRedisplay(viewer);
		break;
	}

	// pass on to scene related keys
	scene_keys (key);
}

void timer (int dummy)
{
	ViewerRedisplay(viewer);
	glutTimerFunc (10, timer, 0);

	// pass on to scene related timer calls
	scene_timer();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	mainwin = glutCreateWindow("Shadow Mapping [xXpr]");
	glutDisplayFunc (display);
	glutReshapeFunc (Reshape);
	viewer = ViewerNew (mainwin, 0,0,512,512,Display);

	ViewerFOVY (viewer, _fovy_);
	glClearColor (.4,.4,.4,.4);

	if(!Init())
		return 0;

	glutKeyboardFunc(Keyboard);
	glutTimerFunc (10, timer, 0); 
	glutMainLoop();
	return 0;
}