#if defined (__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <GLUT/GLUT.h>
#else
	#include <GL/gl.h>
	#include <GL/freeglut.h>
#endif
#include <iostream>
#include <vector>
#include "model.h"
#include <math.h>
using namespace std;

struct point {
	double x,y,z;
};
int r=0;
struct  camara {
	point VRP;
	double phi;
	double theta;
	double d;
	std::vector<double> optica;
};

struct caja { //MINIMUN BOX FOR OBJECT
	double x_max, x_min;
	double y_max, y_min;
	double z_max, z_min;
};

struct esfera {
	double rad;
	point cen;	
};


Model m;
//MODES
bool modeLine, modeRotate;
//
//CONFIG PARAMETERS
double back;
int stacks;
int slices;
//
//INFO
double width; double height; //VIEWPORT SIZE
int x, y; //MOUSE POSITION
//
//OBJECT & SCENE DATA
std::vector<char> v;
esfera escena;
double rx,ry;
//
//cAMARA
camara cam;

void initGL (int argc, const char *argv[])
 {
	glutInit(&argc, (char **)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1000,600);
	glutCreateWindow("IDI: Practiques OpenGL");
	glClearColor(0.25,0.65,0.1,0);
	glEnable(GL_DEPTH_TEST);
}


void pinta_ejes(){
	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3d(0,0,0);
		glVertex3d(1,0,0);
	glEnd();
	glBegin(GL_LINES);
		glColor3d(0,1,0);
		glVertex3d(0,0,0);
		glVertex3d(0,1,0);
	glEnd();
	glBegin(GL_LINES);
		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,1);
	glEnd();
}

void refresh(void) {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
      	
      	//glRotatef(r,0,1,0);

		/*
		glPushMatrix();
			glTranslatef(0,0,0);
			glRotatef(45,0,1,0);
			glColor3f(1,0,0);
			glutSolidTeapot(0.5);
		glPopMatrix();

		glPushMatrix();
			glColor3f(0,0,1);
			glutSolidCube(0.7);
		glPopMatrix();
		*/


		/*glLoadIdentity();
		glTranslatef(0,0,cam.d);
		glRotatef(cam.theta,1,0,0);
		glRotatef(cam.phi,0,1,0);
		glTranslatef(cam.VRP.x,cam.VRP.y,cam.VRP.z);
		*/

		glPushMatrix();
			glColor3f(0,1,0);
		    glTranslatef(5,0,0);
			glutSolidSphere(3,stacks,slices);
		glPopMatrix();

		glPushMatrix();
			glColor3f(1,0,0);
			glTranslatef(12,0,0);
			glutSolidSphere(2,stacks,slices);
		glPopMatrix();

	glPopMatrix();
	r= (r+1)%360; 
	pinta_ejes();
	glutSwapBuffers();
	//glutPostRedisplay();
}
void setViewport(int w, int h)
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double alpha = 18;
	double arv = double(double(h)/double(w));
	arv = (arv>1) ? arv : 1/arv;
	gluPerspective(alpha, arv ,5,26);
	//gluPerspective(17, arv ,5,26);
	gluLookAt(26,10,0,12,0,0,0,0,1);
}

int main(int argc,const char * argv[])
{
	slices = 50;
	stacks = 50;
	
	initGL(argc,argv);
	glutDisplayFunc (refresh);
	glutReshapeFunc(setViewport);	
	glutMainLoop();
	return 0;
}