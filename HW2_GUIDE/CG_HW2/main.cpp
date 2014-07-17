#include "gl/glew.h"
#include "gl/gluit.h"
#include "gl/glpng.h"
#include "svl/svl.h"

#define DRAWTORUS 0
#pragma comment (lib, "glew32.lib")

int mainwin, viewer;
int ww, wh;
bool animate = false;
bool bind = true;
double theta;
//torus color
const float red[]    = {0.5, 0  , 0, 1};
const float green[]  = {0  , 0.3, 0, 1};
const float yellow[] = {1  , 1  , 0, 1};
const float cyan[]   = {0  , 1  , 1, 1};
const float white[]  = {1  , 1  , 1, 1};

GLuint prog;
GLMmodel *m1;
GLuint tex1, tex2;

void display(){}
void reshape(int w, int h)
{
	ww = w;
	wh = h;
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
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

void content()
{
	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);	
	
	Vec3 lightpos = Vec3(1, 1, 1);
	Vec3 lpos = Rot3(Vec3(0, 1, 0), theta) * lightpos;
	
	float pos[4];
	pos[0] = lpos[0];
	pos[1] = lpos[1];
	pos[2] = lpos[2];
	pos[3] = 1.0;

	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	glUseProgram(0);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	glutWireSphere(0.2, 20, 20);
	glPopMatrix();

#if DRAWTORUS
	glMaterialfv(GL_FRONT, GL_AMBIENT, red);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
	glMaterialf(GL_FRONT, GL_SHININESS, 100);
	
	glUseProgram(prog);
	/*glBegin (GL_QUADS);
	glTexCoord2i (0,0); glVertex2i (-1,0);
	glTexCoord2i (2,0); glVertex2i (1,0);
	glTexCoord2i (2,2); glVertex2i (1,2);
	glTexCoord2i (0,2); glVertex2i (-1,2);
	glEnd();*/
	//glEnable(GL_LIGHTING);
	//glutSolidTeapot(1.0);
	glutSolidTorus(0.2, 1.0, 20, 20);
#else
	glUseProgram(prog);
	glmDraw(m1, GLM_SMOOTH | GLM_MATERIAL);
#endif
	
	BEGIN_2D_OVERLAY (ww, wh);
	setfont("9x15", 100);
	
	drawstr(10, 10, "q/Q = Change mode. Press Space to toggle/auto rotate.");
	
	END_2D_OVERLAY();

	glutSwapBuffers();
}

void init()
{
	glEnable (GL_DEPTH_TEST);
	glClearColor(.4, .4, .4, 1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//load OBJ model
	m1 = glmReadOBJ ("al.obj");
	glmUnitize (m1);
	glmFacetNormals (m1);
	glmVertexNormals (m1, 90);

	glClearColor (0.4, 0.4, 0.4, 0.0);
	glShadeModel(GL_FLAT);
	
    makeCheckImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &tex1); // allocate a tex object
    glBindTexture(GL_TEXTURE_2D, tex1);  // bind to Texture Unit #0

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	tex2 = pngBind("pond1.png", PNG_NOMIPMAP, PNG_ALPHA, NULL, GL_REPEAT, GL_NEAREST, GL_NEAREST);
	
	glewInit();
	extern GLuint setShaders (char*, char*);
	prog = setShaders ("pointlight.vert", "pointlight.frag");

	glUseProgram (prog);
	glUniform1i(glGetUniformLocation (prog, "bind"), bind);
	glUniform1i(glGetUniformLocation (prog, "tex1"), 0);
	glUniform1i(glGetUniformLocation (prog, "tex2"), 1);
	
	glActiveTexture(GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, tex1);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);
}

void timer(int dummy)
{
	if(animate)
		theta += 0.1;
	//printf("theta = %.2f\n", theta);
	glutTimerFunc(10, timer, 0);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if(key == ' ')
		animate ^= 1;
	else if(key == 'q' || key == 'Q') {
		bind ^= 1;
		glUniform1i(glGetUniformLocation (prog, "bind"), bind);
	}
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
	init();
	glutTimerFunc(10, timer, 0);
	glutKeyboardFunc(keyboard);
	
	glutMainLoop();
}