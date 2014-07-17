#include <GL/glew.h>			//GLEW lib
#include "gluit.h"			//GLUT lib
#include "svl/svl.h"

//
// note this is NOT Catmull-Rom spline
//
// this is a weird looking Hermite curve, linked together at data points.
// (for demonstration purpose only)

#pragma comment (lib, "glew32.lib")

GLuint p;			//Handlers for our vertex, geometry, and fragment shaders
int mainwin, viewer;

void display(){}
void reshape (int w, int h)
{
	glViewport (0,0,w,h);
	ViewerReshape(viewer);
}

Vec3 datapts[5];

void initdata()
{
	datapts[0] = Vec3(-2,3,0);
	datapts[1] = Vec3(-1,2,0);
	datapts[2] = Vec3(0,1,0);
	datapts[3] = Vec3(1,1.5,0);
	datapts[4] = Vec3(2,2,0);	
}

void content (void) 
{
	glClear(GL_COLOR_BUFFER_BIT);


	glUseProgram(p);

	glBegin(GL_LINES_ADJACENCY_EXT);
	// head
	Vec3 pre = datapts[0] - (datapts[1]-datapts[0]);
	glVertex3dv (pre.Ref());
	glVertex3dv (datapts[0].Ref());
	glVertex3dv (datapts[1].Ref());
	glVertex3dv (datapts[2].Ref());
	
	for (int i = 1; i < 3; i++) { // except head & tail
		glVertex3dv (datapts[i-1].Ref());
		glVertex3dv (datapts[i].Ref());
		glVertex3dv (datapts[i+1].Ref());
		glVertex3dv (datapts[i+2].Ref());

	}
	
	// tail
	glVertex3dv (datapts[2].Ref());
	glVertex3dv (datapts[3].Ref());
	glVertex3dv (datapts[4].Ref());
	Vec3 post = datapts[4] + (datapts[4]-datapts[3]);
	glVertex3dv (post.Ref());
	glEnd();

	// draw data points
	glColor3f(1,0,0);
	glUseProgram(0);	
	glPointSize (10);
	glBegin (GL_POINTS);
	for (i = 0; i < 5; i++)
		glVertex3dv (datapts[i].Ref());
	glEnd();

	glutSwapBuffers();
}

void init() 
{
	initdata();

	extern GLuint setVFGShaders(char*,char*,char*);
	p = setVFGShaders ("hermite.vert", "hermite.frag", "hermite.geom");

	////////////////////////////////
	glProgramParameteriEXT(p,GL_GEOMETRY_INPUT_TYPE_EXT,GL_LINES_ADJACENCY_EXT);
	glProgramParameteriEXT(p,GL_GEOMETRY_OUTPUT_TYPE_EXT,GL_LINE_STRIP);

	int temp;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
	glProgramParameteriEXT(p,GL_GEOMETRY_VERTICES_OUT_EXT,temp);
	////////////////////////

	glLinkProgram(p);

	glUseProgram(p);
	int numloc = glGetUniformLocation (p, "Num");
	glUniform1i (numloc, 6);
}
	
int main(int argc, char **argv) 
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	mainwin = glutCreateWindow ("geometry shader");
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);

	viewer = ViewerNew (mainwin, 0,0,320,320, content);

	glewInit();
	if (glewIsSupported("GL_VERSION_2_1"))
		printf("Ready for OpenGL 2.1\n");
	else {
		printf("OpenGL 2.1 not supported\n");
		exit(1);
	}
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
		printf("Ready for GLSL - vertex, fragment, and geometry units\n");
	else {
		printf("Not totally ready :( \n");
		exit(1);
	}

	glClearColor(1.0,1.0,1.0,1.0);
	glShadeModel(GL_FLAT);
	init();

	glutMainLoop();

	return 0;
}
