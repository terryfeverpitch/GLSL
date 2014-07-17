#include "Food.h"
#include "md2m.h"
#include "vector"

int myFloor, myWall;
int texedobjloc;
int rotate = 1;
int SHADOWMAPTURN = 1, REALTURN = 2;
float ang = 0.0;
float lightPosition[4] = {-1,6,6,1};

vector<Food> shroom;
int total_score;
int hit = -1;
int prog;
GLMmodel *shroom_model;
///////////////////////////////////////////////////////
/// MD2 related variables
float scale, center[3], size[3];
t3DModel *yoshi;

int pose = 1, oldpose;   // 0 (stand), 1 (run), 6 (jump) 
Vec3 point;
float angle;
int loopover;
float speed;

#define max2(x,y) ((x)>(y)?(x):(y))
#define max3(x,y,z) (max2(max2(x,y),max2(y,z)))

void initScene(int program)
{
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	pngSetStandardOrientation(1);
	glActiveTexture(GL_TEXTURE0);
	myFloor = pngBind("checkboard.png", PNG_BUILDMIPMAPS, PNG_SOLID, NULL, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	myWall = pngBind("brick_wall.png", PNG_BUILDMIPMAPS, PNG_SOLID, NULL, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	glUniform1i(glGetUniformLocation(program, "sceneMap"), 0);  // scene use TU #0

	texedobjloc = glGetUniformLocation(program, "textured_object");  // texture_object == 1 for textured object

	// load MD2 model
	yoshi = MD2MReadMD2("yoshi-tris.md2", "yoshi.png");
	MD2MFindDimension(yoshi, center, size);
	scale = max3(size[0],size[1],size[2]);

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

int grab(Vec3 point)  // point: (x,0,z)
{
	double eps = 1.5;  // threshold for successful grabbing
	
	// loop through all un-grabbed tespots
	// if the distance (on XZ plane) is less than the threshold
	//     return the index;
	// when all tespots are examined, return -1 (nothing grabbed)
	//

	for(int i = 0 ; i < shroom.size() ; i++) {
		double distance = sqrt(pow((point[0] - shroom[i].pos[0]), 2) + pow((point[2] - shroom[i].pos[2]), 2));
		if(distance <= eps && shroom[i].status == 1) {
			Vec3 random_position = Vec3(rand() % 20 - 10, 2, rand() % 20 - 10);
			shroom.push_back(Food(random_position));
			cout << "random = " << random_position << endl;
			return i;
		}
	}
	
	return -1;
}

void pose_fsm()
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
			MD2MSetLoop (yoshi, GL_TRUE);
			MD2MSetAnimation (yoshi, pose);
		}
		break;
	}
}

void scene_timer()
{
	ang += 5;
	Food::self_rotation_angle += 2.0;
	//printf("s = %.2f", self_rotation_angle);
	point += Rot3 (Vec3(0,1,0),angle/180*vl_pi) * Vec3(1,0,0)*speed/10;	
	pose_fsm();
}

void get_scene_light_pos(float* pos)
{
	for(int i = 0; i < 4; i++)
		pos[i] = lightPosition[i];
}

void scene_keys(unsigned char key)
{
	switch (key) {	
		case 'X':
			lightPosition[0] += .5;
			break;
		case 'x':
			lightPosition[0] -= .5;
			break;
		case 'r':
			rotate ^= 1;
			break;
	}

	if (key == ' ') {  // stand still
		speed = 0.0;
	} 
	else if(key == 'j' || key == 'J') {
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
	}
}

void DrawScene(int turn)
{
	float white[] = {1.,1.,1.,1.};
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 200.0);
	// floor
	glUniform1i(texedobjloc, 1);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myFloor);
	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0);
		float x = -10.0f, y = 0.0f, z = -10.0f;

		for (GLint i = 0; i < 20; i++, x += 1.0)
		{
			for (GLint j = 0; j < 20; j++, z += 1.0)
			{
			  // draw the plane slightly offset so the shadow shows up  
			  glTexCoord2f(0, 0); glVertex3f(x, 0, z);
			  glTexCoord2f(1, 0); glVertex3f(x + 1.0f, 0, z);
			  glTexCoord2f(1, 1); glVertex3f(x + 1.0f, 0, z + 1.0f);
			  glTexCoord2f(0, 1); glVertex3f(x, 0, z + 1.0f);
			}
			z = -10.0f;
		}
	glEnd();
	// wall
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myWall);
	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0);
		x = -10.0f, z = -10;
		for (GLint i2 = 0; i2 < 20; i2++, x += 1.0)
		{
			for (GLint j2 = 0; j2 < 6; j2++, y += 1.0)
			{
			  glTexCoord2f(0, 0); glVertex3f(x, y, z);
			  glTexCoord2f(1, 0); glVertex3f(x + 1.0f, y, z);
			  glTexCoord2f(1, 1); glVertex3f(x + 1.0f, y + 1.0f, z);
			  glTexCoord2f(0, 1); glVertex3f(x, y + 1.0f, z);
			}
			y = -0.0f;
		}
	glEnd();
	glPopAttrib();
	// yoshi
	glUniform1i(texedobjloc, 1);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(point[0], point[1], point[2]);
	glRotatef(angle, 0,1,0);
	glTranslatef(0,1,0);
	glScalef(2/scale, 2/scale, 2/scale);
	loopover = MD2MInterpolate(yoshi);
	glPopMatrix();
	glPopAttrib();

	glActiveTexture(GL_TEXTURE0);
	// Foods
	if(turn == SHADOWMAPTURN) {
		for(int i = 0 ; i < shroom.size() ; i++) {
				shroom.at(i).shadow();	
		}
	}
	else if(turn == REALTURN) {
		for(int i = 0 ; i < shroom.size() ; i++) {
			if(shroom.at(i).status == -1) {
				shroom.erase(shroom.begin() + i);
			}
			else if(shroom.at(i).status == 1){
				glUniform1i(texedobjloc, 1);	
				shroom.at(i).show();	
			}
		}
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



