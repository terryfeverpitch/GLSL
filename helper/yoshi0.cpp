#include "gl/gluit.h"
#include "svl/svl.h"
#include "md2m.h"
#include "teapot.h"

#pragma comment (lib, "glpngd.lib")

int mainwin, viewer;
void display(){}
void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	ViewerReshape(viewer);
}

void grid()
{
	glPushAttrib (GL_ENABLE_BIT);
	glDisable (GL_LIGHTING);
	glDisable (GL_TEXTURE_2D);
	glColor3f (1,1,1);
	glBegin (GL_LINES);
	for (int i = -10; i <= 10; i++) {
		glVertex3i (-10,0,i); glVertex3i (10,0,i);
		glVertex3i (i,0,-10); glVertex3i (i,0,10);
	}
	glEnd();
	glPopAttrib();
}

//////////////////////////////////////////////////////////////////////
// init
///////////////////////////////////////////////////////
/// MD2 related variables
float scale, center[3], size[3];
t3DModel *yoshi;

int pose, oldpose;   // 0 (stand), 1 (run), 6 (jump) 
Vec3 point;
float angle;
int loopover;
float speed;

#define max2(x,y) ((x)>(y)?(x):(y))
#define max3(x,y,z) (max2(max2(x,y),max2(y,z)))

void init_md2()
{
	// load MD2 model
	yoshi = MD2MReadMD2 ("yoshi-tris.md2", "yoshi.png");
	MD2MFindDimension  (yoshi, center, size);
	scale = max3 (size[0],size[1],size[2]);

	MD2MSetLoop (yoshi, GL_TRUE); 
	MD2MSetAnimation (yoshi, pose);  // 0: stand
	MD2MSetAnimationSpeed (yoshi, 10); 
}

//////////////////////////////////////////////
/// teapot
Teapot teapots[3];
int nteapots = 3;

void initTeapots()
{
	double h = 2.0; // height of teapot
	teapots[0].pos = Vec3 (2,h,3); teapots[0].score = 10;
	teapots[1].pos = Vec3 (0,h,0); teapots[1].score = 10;
	teapots[2].pos = Vec3 (2,h,-3); teapots[2].score = 10;
}
//////////////////////////////////
// grab teapot
int total_score;
int hit = -1;

// Grab: return [0, n-1] for successful grabbing, 
//       return -1 if nothing grabbed
int Grab (Vec3 point)  // point: (x,0,z)
{
	double eps = 1.5;  // threshold for successful grabbing
	
	// loop through all un-grabbed tespots
	// if the distance (on XZ plane) is less than the threshold
	//     return the index;
	// when all tespots are examined, return -1 (nothing grabbed)
	//
	return -1;
}

/////////////////////////////////////////////////
void pose_fsm ()
{
	switch (pose) {
	case 0: // stand
		if (speed > 0) {
			pose = 1;   // stand --> run
			MD2MSetAnimation (yoshi, pose);
		}
		break;
	case 1: // run
		if (speed == 0.0) {
			pose = 0;   // run --> stand
			MD2MSetAnimation (yoshi, pose);
		}
		break;
	case 6: // jump
		if (loopover == 1) {
			pose = oldpose;   // when jump is done --> oldpose (stand or run)
			MD2MSetLoop (yoshi, GL_TRUE);
			MD2MSetAnimation (yoshi, pose);
		}
		break;
	}
}

void content()
{
	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	grid();

	glPushMatrix();
	glTranslated (point[0], point[1],point[2]);
	glRotatef (angle, 0,1,0);

	glTranslatef (0,1,0);
	glScalef (2/scale, 2/scale, 2/scale);
	glTranslatef (-center[0],-center[1],-center[2]);
	loopover = MD2MInterpolate (yoshi);
	glPopMatrix();


	// teapots
	for (int i = 0; i < nteapots; i++) {
		teapots[i].show();
	}

	BEGIN_2D_OVERLAY (10,10);
	glDisable (GL_TEXTURE_2D);
	glColor3f (1,1,0);
	char display[20];
	sprintf (display, "Score: %d", total_score);
	drawstr (1,1, display);
	END_2D_OVERLAY();

	glutSwapBuffers();
}

void init()
{
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_TEXTURE_2D);
//	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);

	init_md2();
	initTeapots();
}


void special (int key, int x, int y)
{
#define CLAMP(v,lo,hi)  ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))
	switch (key) {
		case GLUT_KEY_UP:
			speed += 0.1;
		break;
		case GLUT_KEY_DOWN:
			speed -= 0.1;
		break;
		case GLUT_KEY_RIGHT:
			angle -= 2;
		break;
		case GLUT_KEY_LEFT:
			angle += 2;
		break;
	}
	speed = CLAMP (speed, 0.0, 3.0);
}

void keyboard (unsigned char key, int x, int y)
{
	if (key == ' ') {  // stand still
		speed = 0.0;
	} else if (key == 'j') {
		if (pose != 6) {   // for jump initialization
			MD2MSetLoop (yoshi, GL_FALSE); 
			oldpose = pose;  // save the current pose: stand or walk
			pose = 6;
			MD2MSetAnimation (yoshi, pose);
		}
		hit = Grab (point);
		cout << "hit: " << hit << endl;
		if (hit >= 0) {
			teapots[hit].status = 0;
			total_score += teapots[hit].score;
		}

	}
}


void timer (int dummy)
{
	glutTimerFunc (100, timer, 0);
	point += Rot3 (Vec3(0,1,0),angle/180*vl_pi) * Vec3(1,0,0)*speed/10;	
	pose_fsm();

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
	init();
	glutSpecialFunc (special);
	glutTimerFunc (10, timer, 0);
	glutKeyboardFunc (keyboard);

	glutMainLoop();
}
