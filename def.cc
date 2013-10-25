#if defined (__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <GLUT/GLUT.h>
#else
	#include <GL/gl.h>
	#include <GL/freeglut.h>
#endif
#include <iostream>
#include <vector>
#include <cmath>
#include "model.h"

using namespace std;

#define PI 3.141596

struct point {
	double x,y,z;
};

struct  camara {
	point VRP;
	double phi;
	double theta;
	double d;
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
bool modeRotate, modePres, modeWalls;
//
//CONFIG PARAMETERS
int stacks;
int slices;
//
//INFO
double width = 600; double height = 600; //VIEWPORT SIZE
int x, y; //MOUSE POSITION
//
//OBJECT & SCENE DATA
esfera escena;
double rx=0;
double ry=0;
//
//cAMARA
camara cam;
std::vector<double> optica_ortho;
std::vector<double> optica_pres;
int mx,my;

void initGL (int argc, const char *argv[])
 {
	glutInit(&argc, (char **)argv);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB| GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(width,height);
	glutCreateWindow("IDI: Practiques OpenGL");
	glClearColor(0.9,0.8,0.7,0);
	glEnable(GL_DEPTH_TEST);
}

void conf_camara_ortho () {
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();				
	glOrtho(optica_ortho[0],optica_ortho[1],optica_ortho[2],
	optica_ortho[3],optica_ortho[4],optica_ortho[5]);
	glMatrixMode(  GL_MODELVIEW );
}

void conf_camara_pres () {
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();				
	gluPerspective(optica_pres[0], optica_pres[1] ,optica_pres[2],optica_pres[3]);
	glMatrixMode(GL_MODELVIEW);
}

void ini_camara_optica (){
	double arv = double(double(width)/double(height));
	//OPTICA
	double aux = (arv >= 1)
		? escena.rad*arv 
		: escena.rad/arv;
	optica_ortho[0] = -aux;
	optica_ortho[1] = aux;
	optica_ortho[2] = -aux;
	optica_ortho[3] = aux;
	optica_ortho[4] = cam.d - escena.rad;
	optica_ortho[5] = cam.d + escena.rad;

	optica_pres[0] = (arv < 1.0) 
		? 2*( (atan( tan(asin(escena.rad/cam.d)) /arv) *180)/PI)
		: 2*( (asin(escena.rad/cam.d)*180)/PI);
	optica_pres[1] = arv; 				//arv
	optica_pres[2] = cam.d - escena.rad;//znear;
	optica_pres[3] = cam.d + escena.rad;//zfar;
}

void setViewport(int w, int h) {
	width = w;
	height = h;
	ini_camara_optica();
	if (modePres) conf_camara_pres();
	else conf_camara_ortho();
	glViewport(0,0,w,h);
	glutPostRedisplay();
}

void keyboarEve(unsigned char key, int x, int y) 
{
	if (key == 'h') {
		cout << "HELP" << endl;
		cout << "p -> Cambia entre camara prespectiva y ortho" << endl;
		cout << "q -> Activa/Desactiva rotacion" << endl;
		cout << "v -> Muestra/Oculta las apredes" << endl;
		cout << "ESC -> Cerrar programa" << endl; 
	}
	if (key == 'q') {
		modeRotate = !modeRotate;
		if (modeRotate) cout << "Haz clic y desplaza el raton para rotar la figura - mode 1" << endl;
		else cout << "Modo rotacion desactivado" << endl;
	}
	if (key == 'p') {
		modePres = !modePres;
		//ini_camara_optica();
		if (modePres) {
			conf_camara_pres();
		 	cout << "Camara en prespectiva" << endl;	
		}
		else {
			conf_camara_ortho();
			cout << "Camara orthogonal" << endl;	
		}
		glutPostRedisplay();
	}
	if (key == 'v') {
		modeWalls = !modeWalls;
		if (modeWalls) cout << "Muestra paredes" << endl;
		else cout << "Oculta paredes" << endl;
		glutPostRedisplay();
	}
	if (key == 27) exit(0);
}

void mouseMove (int w, int h){
	if (modeRotate) {
		cam.phi += (x-w)*0.3;
		cam.theta += (y-h)*0.3;
		x = w;
		y = h;
		//cout << cam.theta << "," <<cam.phi << endl;
		glutPostRedisplay();
	}
}

void mouseClick(int b, int r, int xx, int yy){
	if (modePres) {
		if (GLUT_ACTIVE_CTRL && b == 3 && r == GLUT_UP) ++optica_pres[0];
		else if (GLUT_ACTIVE_CTRL && b == 4 && r == GLUT_UP) --optica_pres[0];
		conf_camara_pres();	
	}
	else {
		if (GLUT_ACTIVE_CTRL && b == 3 && r == GLUT_UP) {
			optica_ortho[0] = optica_ortho[2] -= 0.5;
			optica_ortho[1] = optica_ortho[3] += 0.5;
		}
		else if (GLUT_ACTIVE_CTRL && b == 4 && r == GLUT_UP) {
			optica_ortho[0] = optica_ortho[2] += 0.5;
			optica_ortho[1] = optica_ortho[3] -= 0.5;	
		}
		conf_camara_ortho();
	}
	glutPostRedisplay();
	x = xx;
	y = yy;
}


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
	e.rad = 7.1;
	e.cen.x = 0;
	e.cen.y = 0;
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
	return 1.5/l_max;
}

std::vector<double> calc_cen (caja c) {
	std::vector<double> centro_caja (3);
	centro_caja[0]= (c.x_max+c.x_min)/2;
	centro_caja[1]= c.y_min;//(c.y_max+c.y_min)/2;
	centro_caja[2]= (c.z_max+c.z_min)/2;
	return centro_caja;
} 

void snowMan() {
	glPushMatrix();
		glTranslatef(0,0.4,0);

		glPushMatrix();
		glColor3d(1,1,1);
		glutSolidSphere(0.4,stacks,slices);
		glPopMatrix();

		glPushMatrix();
		glColor3d(1,1,1);
		glTranslatef(0,0.6,0);
		glutSolidSphere(0.2,stacks,slices);
		glPopMatrix();

		glPushMatrix();
		glRotated(90,0,1,0);
		glTranslatef(0,0.6,0);
		glTranslatef(0,0,0.1);
		glColor3d(1,0,0);
		glutSolidCone(0.1,0.2,stacks,slices);
		glPopMatrix();
	glPopMatrix();
}

void pinta_model() {
	// Read our .obj file
	const std::vector<Face> f = m.faces();
	const std::vector<Vertex> v = m.vertices();
	caja c = caja_minima (v);
	double esc = calc_escalado (c);
	glScaled(esc,esc,esc);
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

void pinta_terra(){
	glBegin(GL_QUADS);
		glColor3f(0,0.26,0.20);
		glVertex3d(5,0,5);
		glVertex3d(-5,0,5);
		glVertex3d(-5,0,-5);
		glVertex3d(5,0,-5);
	glEnd();
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

void pinta_paret (){
	glPushMatrix();
		glTranslatef(2.5,0.75,-1.5);
		glScaled(4,1.5,0.2);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-4.9,0.75,0);
		glScaled(0.2,1.5,10);
		glutSolidCube(1);
	glPopMatrix();
}

void refresh(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
		glTranslatef(0,0,-cam.d);
		glRotatef(cam.theta,1,0,0);
		glRotatef(cam.phi,0,1,0);
		glTranslatef(-cam.VRP.x,-cam.VRP.y,-cam.VRP.z);
		
		glPushMatrix();
			glTranslatef(2.5,0,2.5);
			snowMan();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.5,0,2.5);
			snowMan();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.5,0,-2.5);
			snowMan();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(2.5,0,-2.5);
			snowMan();
		glPopMatrix();

		glPushMatrix();
			if (modeWalls) pinta_paret();
		glPopMatrix();

		glPushMatrix();
			pinta_model();
		glPopMatrix();
		
		glPushMatrix();
			pinta_terra();
		glPopMatrix();

	glPopMatrix();

	glPushMatrix();
		pinta_ejes();
	glPopMatrix();

	glutSwapBuffers();
}

int main(int argc,const char * argv[])
{
	initGL(argc,argv);
	m.load("porsche.obj");
	slices = 50;
	stacks = 50;
	modeRotate = false;
	modePres = false;
	modeWalls = false;
	escena = esfera_cont_esce();
	//POS
	cam.VRP.x = escena.cen.x;
	cam.VRP.y = escena.cen.y;
	cam.VRP.z = escena.cen.z;
	cam.theta = 45;
	cam.phi = 45;
	cam.d = 20;
	std::vector<double> va(6);
	std::vector<double> vaa(4);
	optica_ortho = va;
	optica_pres = vaa;
	ini_camara_optica();
	conf_camara_ortho();
	glutDisplayFunc (refresh);
	glutReshapeFunc(setViewport);	
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboarEve);
	glutMainLoop();
	return 0;
}