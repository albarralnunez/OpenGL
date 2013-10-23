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

using namespace std;

struct point {
	double x,y,z;
};

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
	glutInitWindowSize(600,600);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glutCreateWindow("IDI: Practiques OpenGL");
}

void conf_camara () {
	glMatrixMode(GL_PROJECTION);	//
	glLoadIdentity();				//
	//glOrtho(cam.optica[0],cam.optica[1],cam.optica[2],
	//cam.optica[3],cam.optica[4],cam.optica[5]);		//
	gluPerspective(45.23, 1,7,23);
	gluLookAt(-7,0,0,9,0,0,0,1,0);
	glMatrixMode(GL_MODELVIEW);		//	
}

void setViewport(int w, int h)
{
	double rav = h/w; 
	cam.optica[0] = -escena.rad*rav;
	cam.optica[1] = escena.rad*rav;
	cam.optica[2] = -escena.rad*rav;
	cam.optica[3] = escena.rad*rav;
	conf_camara();
}

void keyboarEve(unsigned char key, int xx, int y) 
{
	if (key == 'h') {
		cout << " HELP" << endl;
		cout << "x -> Cambiar entre modo de vision" << endl;
		cout << "ESC -> Cerrar programa" << endl; 
	}
	if (key == 'x') {
		modeLine = !modeLine;
		glutPostRedisplay();	
	}
		if (key == 'r') {
		modeRotate = !modeRotate;
		if (modeRotate) cout << "Haz clic y desplaza el raton para rotar la figura - mode 1" << endl;
		else cout << "Modo rotacion desactivado" << endl;
	}
	if (key == 27) exit(0);
}

void mouseMove (int w, int h){
	if (modeRotate) {
		if (x >= 0) {
			if (w > x) rx += 1;
			if (w < x) rx -= 1;
		}
		if (y >= 0) {
			if (h > y) ry += 1;
			if (h < y) ry -= 1;
		}
		glutPostRedisplay();
	}
}
void mouseClick(int b, int r, int xx, int yy){}


caja caja_minima (const std::vector<Vertex> &v) {
	caja c;
	c.x_max = v[0], c.x_min = v[0];
	c.y_max = v[1], c.y_min = v[1];
	c.z_max = v[2], c.z_min = v[2];
	for (int i = 3; i < v.size(); ++i) {
		if (i%3 == 0) {
				if (c.x_max < v[i]) c.x_max = v[i];
				if (c.x_min > v[i]) c.x_min = v[i];
		}
		if (i%3 == 1) {
				if (c.y_max < v[i]) c.y_max = v[i];
				if (c.y_min > v[i]) c.y_min = v[i];
		}
		if (i%3 == 2) {
				if (c.z_max < v[i]) c.z_max = v[i];
				if (c.z_min > v[i]) c.z_min = v[i];
		}
	}
	return c;
}

esfera esfera_cont_esce(){
	esfera e;
	e.rad = 1.06;
	e.cen.x = 0;
	e.cen.y = -0.4;
	e.cen.z = 0;
	return e;
}

double lado_max (double x,double y, double z) {
	if (x >= y and x >= z) return x;
	if (y >= x and y >= z) return y;
	if (z >= y and z >= x) return z;
}

double calc_escalado(caja c) {
	double l_max = lado_max(c.x_max-c.x_min,c.y_max-c.y_min,c.z_max-c.z_min );
	return 2/l_max;
}

std::vector<double> calc_cen (caja c) {
	std::vector<double> centro_caja (3);
	centro_caja[0]= (c.x_max+c.x_min)/2;
	centro_caja[1]= c.y_min;//(c.y_max+c.y_min)/2;
	centro_caja[2]= (c.z_max+c.z_min)/2;
	return centro_caja;
} 

void pinta_model() {
	// Read our .obj file
	const std::vector<Face> f = m.faces();
	const std::vector<Vertex> v = m.vertices();
	caja c = caja_minima (v);
	double esc = calc_escalado (c);
	glScaled(0.25*esc,0.25*esc,0.25*esc);
	vector<double> centro_caja = calc_cen (c);
	glTranslatef(-centro_caja[0],-centro_caja[1],-centro_caja[2]);
	for (int i = 0; i < f.size(); ++i) {
		glBegin(GL_TRIANGLES);
			glColor3f(Materials[f[i].mat].diffuse[0],Materials[f[i].mat].diffuse[1],
				Materials[f[i].mat].diffuse[2]);
			glVertex3dv(&m.vertices()[f[i].v[0]]);
			glVertex3dv(&m.vertices()[f[i].v[1]]);
			glVertex3dv(&m.vertices()[f[i].v[2]]);
		glEnd();
	}
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
	glClearColor(0.25,0.65,0.1,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!modeLine) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glMatrixMode (GL_MODELVIEW);
	conf_camara();
	glLoadIdentity();
	glPushMatrix();

		glLoadIdentity();
		glTranslatef(0,0,cam.d);
		glRotatef(cam.theta,1,0,0);
		glRotatef(cam.phi,0,1,0);
		glTranslatef(cam.VRP.x,cam.VRP.y,cam.VRP.z);

		glPushMatrix();
		    glTranslatef(5,0,0);
			glutSolidSphere(3,stacks,slices);
		glPopMatrix();

		glPushMatrix();
			glColor3f(1,0,0);
			glTranslatef(12,0,0);
			glutSolidSphere(2,stacks,slices);
		glPopMatrix();

	glPopMatrix();

	pinta_ejes();
	glutSwapBuffers();
}

int main(int argc,const char * argv[])
{
	slices = 50;
	stacks = 50;
	modeLine = false;
	modeRotate = false;
	escena = esfera_cont_esce();
	cam.VRP.x = cam.VRP.y = cam.VRP.z = 0;
	cam.theta = 0;
	cam.phi = 0;
	cam.d = 0;
	std::vector<double> vaux(6);
	vaux[0] = vaux[2] = vaux[4] = 1;
	vaux[1] = vaux[3] = vaux[5] = -1;
	cam.optica = vaux;
	initGL(argc,argv);
	glutDisplayFunc (refresh);
	glutReshapeFunc(setViewport);	
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboarEve);
	glutMainLoop();
	return 0;
}