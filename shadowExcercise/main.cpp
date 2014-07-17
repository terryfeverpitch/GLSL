#include <gl/glew.h>
#include "gluit.h"
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

// light positions: static; used in this file
static float lightPosition[4];
float _fovy_ = 95.0;

// not needed for GLSL
// Texture object for shadow texture
// GLuint shadowMapTexture;

// viewport size
int windowWidth = 512;
int windowHeight = 512;

// shadow map size
int shadowMapSize=512;

void display(void){}

void initShadowmapShaders(void)
{
	extern GLuint setShaders(char*,char*);
	program = setShaders ("shadowmap.vert", "shadowmap.frag");
	glUseProgram (program);
	glUniform1i (glGetUniformLocation (program, "shadowMap"), 3);   // set shadowmap to texture unit 3

	glUseProgram(0);   // disable shader for now
}

void init_shadowmap()
{
	//Create the shadow map texture (to texture unit 3)
	glActiveTexture (GL_TEXTURE3);
	GLuint shadowMapTexture;
	glGenTextures(1, &shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0,
					GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

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
	// We use glScale when drawing the scene
	// glEnable(GL_NORMALIZE);

	init_shadowmap();
	
	initScene(program);

	return true;
}

void apply_modelview_inverse (void)
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

// light position is now in scene.cpp
// Use this routine to get it.
void SetWorldLight ()
{
	get_scene_light_pos (lightPosition);
	glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
}


#define MULT_LIGHT_PROJECTION()  gluPerspective(_fovy_, 1.0f, 2.0f, 8.0f)
#define MULT_LIGHT_MODELVIEW()   gluLookAt(lightPosition[0],lightPosition[1],lightPosition[2],\
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f)
			
void SHADOW_MAP_PASS()   // NON-FBO Version
{
	glClear(GL_DEPTH_BUFFER_BIT);  // no output to colorbuffer

	glViewport(0, 0, shadowMapSize, shadowMapSize);
	glMatrixMode(GL_PROJECTION); glPushMatrix();   glLoadIdentity();// save current projection
	MULT_LIGHT_PROJECTION();  // light projection

	glMatrixMode(GL_MODELVIEW);	glPushMatrix();  glLoadIdentity(); // viewer: save current modelview (containing Viewing of camera)
	MULT_LIGHT_MODELVIEW();   // light view

	// disable texturing in the first pass (if any)
	// disable lighting too

	glPushAttrib (GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
		//Disable color writes, and use flat shading for speed
		glColorMask(0, 0, 0, 0);  // color buffer bit
		glShadeModel(GL_FLAT);    // lighting bit
		glDisable (GL_LIGHTING);
		glDisable (GL_TEXTURE_2D);

		glPolygonOffset (8.0, 4.0); // IMPORTANT: to remove moire pattern (from z-fighting)
		glEnable (GL_POLYGON_OFFSET_FILL);
		
		glUseProgram(0);
		DrawScene(); // Draw the scene (fixed function)
	glPopAttrib();

	glMatrixMode(GL_PROJECTION); glPopMatrix();  // restore current projection
	glMatrixMode(GL_MODELVIEW); glPopMatrix();  // restore Modelview to Vcamera

	//Read the depth buffer into the shadow map texture
	glActiveTexture (GL_TEXTURE3);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);
}

void RENDERING_PASS ()
{
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
	apply_modelview_inverse ();  // camera view inverse 

	glMatrixMode (GL_MODELVIEW);
	
	// three important settings to use depth texture (see notes)

	glUseProgram (program);
	DrawScene();
}

void ShowLight()
{
	// show light ...
	glUseProgram (0);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix(); 
	glTranslatef (lightPosition[0],lightPosition[1],lightPosition[2]);
	glColor3ub (255,255,0); glutSolidSphere (0.1,12,12);
	glPopMatrix();
}

void Display(void)
{
	SetWorldLight();  

	SHADOW_MAP_PASS();

	RENDERING_PASS();
	
	// FOR visual verification
	glUseProgram(0);
	ShowLight();
	
	glutSwapBuffers();
}

void Reshape (int w, int h)
{
	//Save new window size
	windowWidth=w, windowHeight=h;

	glViewport (0,0,w,h);
	ViewerReshape(viewer);
}

//Called when a key is pressed
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27: // escape
		exit(0);
		break;
	}

	// pass on to scene related keys
	scene_keys (key);
}

void timer (int dummy)
{
	ViewerRedisplay(viewer);
	glutTimerFunc (100, timer, 0);

	// pass on to scene related timer calls
	scene_timer();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	mainwin = glutCreateWindow("Shadow Mapping [xX]");
	glutDisplayFunc (display);
	glutReshapeFunc (Reshape);
	viewer = ViewerNew (mainwin, 0,0,512,512,Display);

	ViewerFOVY (viewer, _fovy_);
	glClearColor (.4,.4,.4,.4);

	if(!Init())
		return 0;

	glutKeyboardFunc(Keyboard);
	glutTimerFunc (100, timer, 0); 
	glutMainLoop();
	return 0;
}