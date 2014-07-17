#include <gl/glew.h>
#include <gl/glut.h>

#define shadowMapSize 400
#define NULL 0
extern float lightPosition[4];

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

//#define MULT_LIGHT_PROJECTION()  gluPerspective(_fovy_, 1.0f, 2.0f, 8.0f)
#define MULT_LIGHT_PROJECTION() glFrustum (-2,2,-2,2,2,8)
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
		//DrawScene(); // Draw the scene (fixed function)
		extern void scene();
		scene();
	glPopAttrib();

	glMatrixMode(GL_PROJECTION); glPopMatrix();  // restore current projection
	glMatrixMode(GL_MODELVIEW); glPopMatrix();  // restore Modelview to Vcamera

	//Read the depth buffer into the shadow map texture
	glActiveTexture (GL_TEXTURE3);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);
}
