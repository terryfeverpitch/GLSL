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

void grid()
{
	glBegin (GL_LINES);
	for (int i = -3; i <= 3; i++) {
		glVertex3i (-3,0,i); glVertex3i (3,0,i);
		glVertex3i (i,0,-3); glVertex3i (i,0,3);
	}
	glEnd();
}

GLuint p;

void content()
{
	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	grid();

	//glUseProgram (p);
	glBegin (GL_QUADS);
	glTexCoord2i (0,0); glVertex2i (-1,0);
	glTexCoord2i (2,0); glVertex2i (1,0);
	glTexCoord2i (2,2); glVertex2i (1,2);
	glTexCoord2i (0,2); glVertex2i (-1,2);
	glEnd();
	glUseProgram (0);

	glutSwapBuffers();
}

#define checkImageHeight 64
#define checkImageWidth  64

GLubyte checkImage[checkImageWidth][checkImageHeight][4];
void makeCheckImage(void)
{
   int i, j, c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}

GLuint texName;

void init(void)
{    
	glewInit();
	glEnable (GL_DEPTH_TEST);

	extern GLuint setShaders (char*, char*);
	p = setShaders ("simpletex.vert", "simpletex.frag");

	glUseProgram (p);
	glUniform1i (glGetUniformLocation (p, "tex"), 0);
	glUseProgram (0);

   glClearColor (0.4, 0.4, 0.4, 0.0);
   glShadeModel(GL_FLAT);
   glEnable(GL_DEPTH_TEST);

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);  // bind to Texture Unit #0

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 
                0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (400,400);
	mainwin = glutCreateWindow ("Simple Texturing");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew (mainwin, 0,0,400,400, content);
	init();


	glutMainLoop();
}
