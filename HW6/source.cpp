#include "gl/gluit.h"
#include "svl/svl.h"
#include "windows.h"
#include <vector>

typedef struct color_s {
	float r;
	float g;
	float b;
} COLOR;

typedef struct Teapot_s {
	Vec3 pos;
	COLOR color;
} Teapot;

COLOR colorList[5] = {{1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}};

vector<Teapot> teapots;

LARGE_INTEGER freq, before, after;

float spec[4] = {1, 1, 1, 1};

int mainwin, viewer;

void display(){}

void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}

void content()
{
	QueryPerformanceCounter(&before);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for(int i = 0 ; i < teapots.size() ; i++) {
		float color[3] = {teapots[i].color.r, teapots[i].color.g, teapots[i].color.b}; 
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glPushMatrix();
		glTranslatef(teapots[i].pos[0], teapots[i].pos[1], teapots[i].pos[2]);
		glutSolidTeapot(0.5);
		glPopMatrix();
	}
	
	glutSwapBuffers();
	QueryPerformanceCounter(&after);

	double time = ((double)after.QuadPart - before.QuadPart) / freq.QuadPart;
	printf("elapsed time = %f\n", time);
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	Vec3 p(0, 0, 0);
	float c[3] = {1, 1, 1};
	Teapot _teapot;

	_teapot.pos = p;
	
	int colorIndex = 0;
	for(float i = -3 ; i <= 3 ; i += 1.5) {
		_teapot.pos[0] = i;
		for(float j = -3 ; j <= 3 ; j += 1.5) {
			_teapot.color = colorList[colorIndex];
			_teapot.pos[2] = j;
			teapots.push_back(_teapot);
		}
		colorIndex += 1;
	}
	
	QueryPerformanceFrequency(&freq);

	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT, GL_SHININESS, 300);
}

void main (int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(400,400);
	mainwin = glutCreateWindow("tst");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	viewer = ViewerNew(mainwin, 0,0,400,400, content);
	init();
	glutMainLoop();
}
