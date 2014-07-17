#include "Food.h"
#include "md2m.h"
#include "math.h"
#include "vector"

#define SHADOW_TURN 1
#define REAL_TURN 2

#pragma comment (lib, "glpngd.lib")
#pragma comment (lib, "glew32.lib")

static float lightPosition[4] = {-3.0, 8.0, 3.0, 1.0};

vector<Food> shroom;
int total_score;
int hit = -1;

GLMmodel *shroom_model;
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

void initOccluder()
{
	// load MD2 model
	yoshi = MD2MReadMD2 ("yoshi-tris.md2", "yoshi.png");
	MD2MFindDimension  (yoshi, center, size);
	scale = max3 (size[0],size[1],size[2]);

	MD2MSetLoop(yoshi, GL_TRUE); 
	MD2MSetAnimation(yoshi, pose);  // 0: stand
	MD2MSetAnimationSpeed (yoshi, 10); 

	double h = 3.0; // height of foods

	shroom_model = glmReadOBJ("shroom.obj");
	glmUnitize(shroom_model);
	glmFacetNormals(shroom_model);
	glmVertexNormals(shroom_model, 90);

	Food::food_model = shroom_model;

	shroom.push_back(Food(Vec3(2,h,3)));
	shroom.push_back(Food(Vec3(0,h,0)));
	shroom.push_back(Food(Vec3(2,h,-3)));
}

static GLuint FloorTexture;

void initFloor(char* filename)
{
	pngSetStandardOrientation (1);
	FloorTexture = pngBind(filename, PNG_BUILDMIPMAPS, PNG_SOLID, NULL, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
}

int grab(Vec3 point)  // point: (x,0,z)
{
	double eps = 1.5;  // threshold for successful grabbing

	for(int i = 0 ; i < shroom.size() ; i++) {
		double distance = sqrt(pow((point[0] - shroom[i].pos[0]), 2) + pow((point[2] - shroom[i].pos[2]), 2));
		if(distance <= eps && shroom[i].status == 1) {
			Vec3 random_position = Vec3(rand() % 20 - 10, 3, rand() % 20 - 10);
			shroom.push_back(Food(random_position));
			cout << "random = " << random_position << endl;
			return i;
		}
	}
	
	return -1;
}

void pose_fsm ()
{
	switch(pose) {
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
			MD2MSetLoop(yoshi, GL_TRUE);
			MD2MSetAnimation(yoshi, pose);
		}
		break;
	}
}

void DrawOccluder(int turn)
{
	glPushAttrib(GL_ENABLE_BIT);

	// Foods
	if(turn == SHADOW_TURN) {
		for(int i = 0 ; i < shroom.size() ; i++) {
			shroom.at(i).shadow();
		}
	}
	else if(turn == REAL_TURN) {
		for(int i = 0 ; i < shroom.size() ; i++) {
			shroom.at(i).show();
			if(shroom.at(i).status == -1) {
				shroom.erase(shroom.begin() + i);
			}
		}
	}

	glPushMatrix();
	glTranslated(point[0], point[1],point[2]);
	glRotatef(angle, 0,1,0);
	glTranslatef(0,1,0);
	glScalef(2/scale, 2/scale, 2/scale);
	loopover = MD2MInterpolate(yoshi);
	glPopMatrix();

#if 0 // test texture obj model
	glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		if(turn == SHADOW_TURN)
			glDisable(GL_LIGHTING);
		else {
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
		}
		glPushMatrix();
		glTranslated(6, 1, 4);
		glmDraw(shroom_model, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();
	glPopAttrib();
#endif

	glPopAttrib();
}

void DrawFloor(float fCenterX, float fCenterY, float fCenterZ)
{
	float FloorColor[] = {1,1,1,1};
	glMaterialfv(GL_FRONT, GL_SPECULAR, FloorColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, FloorColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 200.0);

	// set up blending so we can see the reflected cube through the
	// surface, and thus create the illusion of reflection

	glBindTexture(GL_TEXTURE_2D, FloorTexture);

	glPushAttrib(GL_ENABLE_BIT);
	//glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0);
		glTexCoord2f(0, 0); glVertex3f(fCenterX-10, fCenterY, fCenterZ+10);
		glTexCoord2f(1, 0); glVertex3f(fCenterX+10, fCenterY, fCenterZ+10);
		glTexCoord2f(1, 1); glVertex3f(fCenterX+10, fCenterY, fCenterZ-10);
		glTexCoord2f(0, 1); glVertex3f(fCenterX-10, fCenterY, fCenterZ-10);
	glEnd();
	glPopAttrib();
}

void scene_timer ()
{
	Food::self_rotation_angle += 2.0;
	point += Rot3 (Vec3(0,1,0),angle/180*vl_pi) * Vec3(1,0,0)*speed/10;	
	pose_fsm();
}
void scene_keys (unsigned char key)
{
    switch (key) {
		case 'x':
			lightPosition[0] -= 0.1;
			break;

		case 'X':
			lightPosition[0] += 0.1;
			break;

		case 'j': case 'J':
			if(pose != 6) {   // for jump initialization
				MD2MSetLoop (yoshi, GL_FALSE); 
				oldpose = pose;  // save the current pose: stand or walk
				pose = 6;
				MD2MSetAnimation(yoshi, pose);
			}

			hit = grab(point);

			if (hit >= 0) {
				shroom[hit].status = 0;
				total_score += shroom[hit].score;
			}
			break;

		case ' ':
			speed = 0.0;
			break;
    }
}

void scene_special(int key)
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

void get_scene_light_pos(float* pos)
{
    for(int i = 0; i < 4; i++)
		pos[i] = lightPosition[i];
}
