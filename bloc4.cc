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

struct car {
	point cen;
	int ang;
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
bool modeRotate = false;
bool modePres = false;
bool modeWalls = false;
bool modeFP = false;
bool modeLight0 = false;
bool modeLight1 = false;
bool modeLight2 = false;
bool modeVertex = false;
int lightVertex = 0;
bool modeLight = false;
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

car coche;
camara cam;
std::vector<double> optica_ortho;
std::vector<double> optica_pres;
int mx,my;

//RANDOM
float vecColor[4];
double vecNormal[3];
float vecPos[4];

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
	glEnable(GL_NORMALIZE);
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
	optica_ortho[0] = (arv > 1) ? -escena.rad*arv : -escena.rad; 	//LEFT
	optica_ortho[1] = (arv > 1) ? escena.rad*arv : escena.rad;//RIGHT
	optica_ortho[2] = (arv > 1) ? -escena.rad : -escena.rad/arv;//BOT
	optica_ortho[3] = (arv > 1) ? +escena.rad : +escena.rad/arv;////TOP
	optica_ortho[4] = cam.d - escena.rad;
	optica_ortho[5] = cam.d + escena.rad;

	optica_pres[0] = (arv < 1.0) 
		? 2*( (atan( tan(asin(escena.rad/cam.d)) /arv) *180)/PI)
		: 2*( (asin(escena.rad/cam.d)*180)/PI);
	optica_pres[1] = arv; 				//arv
	optica_pres[2] = cam.d - escena.rad;//znear;
	optica_pres[3] = cam.d + escena.rad;//zfar;
}

void firstPersonView () {
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();
	gluPerspective(80,1,0.1,20);
	gluLookAt(coche.cen.x,coche.cen.y+0.5,coche.cen.z,	//OBS
		coche.cen.x+sin(coche.ang*PI/180), 				//VRP
		coche.cen.y+0.2,
		coche.cen.z+cos(coche.ang*PI/180), 				//up
		0,1,0);			
	glMatrixMode(GL_MODELVIEW);
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
		cout
		<< "HELP" << endl
		<< "[" << modePres << "]" 
		<< "	p -> Activa/Desactiva camara en prespectiva" << endl
		<< "[" << modeRotate << "]" 
		<< "	q -> Activa/Desactiva rotacion" << endl
		<< "[" << modeWalls << "]" 
		<< "	v -> Muestra/Oculta las apredes" << endl
		<< "[" << modeFP << "]" 
		<< "	m -> Camara en primera persona" << endl
		<< "[" << modeLight0 << "]" 
		<< "	x -> Activa/Desactiva iluminacion" << endl
		<< "[" << modeLight << "]" 
		<< "	1 -> Activa/Desactiva luz de camara" << endl
		<< "[" << modeLight1 << "]" 
		<< "	2 -> Activa/Desactiva luz en la esquina" << endl
		<< "[" << lightVertex << "]" 
		<< "	f -> Cambia la posicion de la luz de la esquina" << endl
		<< "[" << modeLight2 << "]" 
		<< "	3 -> Activa/Desactiva luz en el coche" << endl
		<< "[" << modeVertex << "]" 
		<< "	b -> Activa/Desactiva normal por vertices" << endl
		<< "	r -> Resetea camara" << endl
		<< "	ESC -> Cerrar programa" << endl
		<< "	Rueda del raton -> Zoom in/Zoom out" << endl
		<< "	'w','a','s', 'd' para mover el coche" << endl;
	}
	if (key == 'b') {
		modeVertex = !modeVertex;
		glutPostRedisplay();
	}
	if (key == 'q') {
		modeRotate = !modeRotate;
	}
	if (key == 'p') {
		modePres = !modePres;
		//ini_camara_optica();
		if (modePres) {
			conf_camara_pres();
		}
		else {
			conf_camara_ortho();
		}
		glutPostRedisplay();
	}
	if (key == 'v') {
		modeWalls = !modeWalls;
		glutPostRedisplay();
	}
	if (key == 'r') {
		ini_camara_optica();
		if (modePres) conf_camara_pres();
		else conf_camara_ortho();
		cam.theta = 45;
		cam.phi = 45;
		modeLight1 = false,
		modeLight0 = false;
		modeLight2 = false;
		modeLight = false;
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glutPostRedisplay();
	}
	if (key == 'm') {
		modeFP = !modeFP;
		if (modeFP) firstPersonView();
		else {
			ini_camara_optica();
			conf_camara_pres();
		}
		glutPostRedisplay();	
	}
	if(key == 'w'){
	  coche.cen.z += cos(((coche.ang*PI)/180))*0.1; 
	  coche.cen.x += sin(((coche.ang*PI)/180))*0.1;

	}
	if(key == 's'){
	  coche.cen.z -= cos(((coche.ang*PI)/180))*0.1; 
	  coche.cen.x -= sin(((coche.ang*PI)/180))*0.1;
	}
	if(key == 'a') coche.ang += 2;
	if(key == 'd') coche.ang += -2;
	if (key == 'a' or key == 's' or key == 'w' or key == 'd') glutPostRedisplay();

	if(key == '1') {
		modeLight1 = !modeLight1;
		if(modeLight1) glEnable(GL_LIGHT1);
		else glDisable(GL_LIGHT1); 
		glutPostRedisplay();
	}
	if(key == '2') {
		modeLight0 = !modeLight0;
		if(modeLight0) glEnable(GL_LIGHT0);
		else glDisable(GL_LIGHT0); 
		glutPostRedisplay();
	}
	if(key == '3') {
		modeLight2 = !modeLight2;
		if(modeLight2) glEnable(GL_LIGHT2);
		else glDisable(GL_LIGHT2);
		glutPostRedisplay();
	}
	if (key == 'f') {
		++lightVertex;
		if (lightVertex == 4) lightVertex = 0;
		glutPostRedisplay();	
	}
	if (key == 'x') {
		modeLight = !modeLight;
		if(modeLight) glEnable(GL_LIGHTING);
		else glDisable(GL_LIGHTING);
		glutPostRedisplay();
	}
	


	if(key == 27) exit(0);
}

void mouseMove (int w, int h){
	if (modeRotate) {
		cam.phi += (x-w)*0.3;
		cam.theta += (y-h)*0.3;
		x = w;
		y = h;
		glutPostRedisplay();
	}
}

void mouseClick(int b, int r, int xx, int yy){
	double rat = width/height;
	if (modePres) {
		if (GLUT_ACTIVE_CTRL && b == 3 && r == GLUT_UP && optica_pres[0] < 170)
			++optica_pres[0];
		else if (GLUT_ACTIVE_CTRL && b == 4 && r == GLUT_UP && optica_pres[0] > 0)
			--optica_pres[0];
		conf_camara_pres();	
	}
	else {
		if (GLUT_ACTIVE_CTRL && b == 3 && r == GLUT_UP ) {
			optica_ortho[0] = (rat > 1) ? optica_ortho[0]+(0.5*rat) : optica_ortho[0]+0.5;
			optica_ortho[1] = (rat > 1) ? optica_ortho[1]-(0.5*rat) : optica_ortho[1]-0.5;
			optica_ortho[2] = (rat > 1) ? optica_ortho[2]+0.5 : optica_ortho[2]+(0.5/rat);
			optica_ortho[3] = (rat > 1) ? optica_ortho[3]-0.5 : optica_ortho[3]-(0.5/rat);
		}
		else if (GLUT_ACTIVE_CTRL && b == 4 && r == GLUT_UP) {
		optica_ortho[0] = (rat > 1) ? optica_ortho[0]-(0.5*rat) : optica_ortho[0]-0.5;
		optica_ortho[1] = (rat > 1) ? optica_ortho[1]+(0.5*rat) : optica_ortho[1]+0.5;
		optica_ortho[2] = (rat > 1) ? optica_ortho[2]-0.5 : optica_ortho[2]-(0.5/rat);
		optica_ortho[3] = (rat > 1) ? optica_ortho[3]+0.5 : optica_ortho[3]+(0.5/rat);
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
		glTranslatef(0,0.4,0);
		glPushMatrix();
			glColor3d(1,1,1);
			vecColor[0] = vecColor[1] = vecColor[2] = 0.5;
			glMaterialfv(GL_FRONT,GL_AMBIENT, vecColor);
			glMaterialfv(GL_FRONT,GL_DIFFUSE, vecColor);
			vecColor[0] = vecColor[1] = vecColor[2] = 1;
			glMaterialfv(GL_FRONT,GL_SPECULAR, vecColor);
			glMateriali(GL_FRONT,GL_SHININESS, 100);
			glutSolidSphere(0.4,stacks,slices);
		glPopMatrix();
		glPushMatrix();
			glColor3d(1,1,1);
			glTranslatef(0,0.6,0);
			vecColor[0] = vecColor[1] = vecColor[2] = 0.5;
			glMaterialfv(GL_FRONT,GL_AMBIENT, vecColor);
			glMaterialfv(GL_FRONT,GL_DIFFUSE, vecColor);
			vecColor[0] = vecColor[1] = vecColor[2] = 1;
			glMaterialfv(GL_FRONT,GL_SPECULAR, vecColor);
			glMateriali(GL_FRONT,GL_SHININESS, 100);
			glutSolidSphere(0.2,stacks,slices);
		glPopMatrix();	
		glPushMatrix();
			glRotated(90,0,1,0);
			glTranslatef(0,0.6,0);
			glTranslatef(0,0,0.1);
			glColor3d(1,0,0);
			vecColor[0]= 230;vecColor[1]= 95;vecColor[2]= 0;
			glMaterialfv(GL_FRONT,GL_AMBIENT, vecColor);
			glMaterialfv(GL_FRONT,GL_DIFFUSE, vecColor);
			vecColor[0] = vecColor[1] = vecColor[2] = 1;
			glMaterialfv(GL_FRONT,GL_SPECULAR, vecColor);
			glMateriali(GL_FRONT,GL_SHININESS, 100);
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
			if (!modeVertex) {
				glNormal3dv(f[i].normalC);
				glMaterialfv(GL_FRONT,GL_AMBIENT , Materials[f[i].mat].ambient);
				glMaterialfv(GL_FRONT,GL_DIFFUSE, Materials[f[i].mat].diffuse);
				glMaterialfv(GL_FRONT,GL_SPECULAR, Materials[f[i].mat].specular);
				glMateriali(GL_FRONT,GL_SHININESS, Materials[f[i].mat].shininess);
				glVertex3dv(&m.vertices()[f[i].v[0]]);
				glVertex3dv(&m.vertices()[f[i].v[1]]);
				glVertex3dv(&m.vertices()[f[i].v[2]]);
			}	
			else  {
				glMaterialfv(GL_FRONT,GL_AMBIENT , Materials[f[i].mat].ambient);
				glMaterialfv(GL_FRONT,GL_DIFFUSE, Materials[f[i].mat].diffuse);
				glMaterialfv(GL_FRONT,GL_SPECULAR, Materials[f[i].mat].specular);
				glMateriali(GL_FRONT,GL_SHININESS, Materials[f[i].mat].shininess);
				glNormal3dv(&m.normals()[f[i].n[0]]);
				glVertex3dv(&m.vertices()[f[i].v[0]]);
				glNormal3dv(&m.normals()[f[i].n[1]]);
				glVertex3dv(&m.vertices()[f[i].v[1]]);
				glNormal3dv(&m.normals()[f[i].n[2]]);
				glVertex3dv(&m.vertices()[f[i].v[2]]);	
			}
		glEnd();
	}
}

void pinta_terra(){
	glPushMatrix();
	glColor3f(0,0.26,0.20);
	vecNormal[0] = 0;
	vecNormal[1] = 1;
	vecNormal[2] = 0;
	glNormal3dv(vecNormal);
	vecColor[0] = 0;vecColor[1] = 0;vecColor[2] = 100;
	glMaterialfv(GL_FRONT,GL_AMBIENT, vecColor);
	vecColor[0] = 0;vecColor[1] = 0;vecColor[2] = 150;
	glMaterialfv(GL_FRONT,GL_DIFFUSE, vecColor);
	vecColor[0] = 0;vecColor[1] = 0;vecColor[2] = 1;
	glMaterialfv(GL_FRONT,GL_SPECULAR, vecColor);
	glMateriali(GL_FRONT,GL_SHININESS, 64);
	glBegin(GL_QUADS);
		glVertex3d(5,0,5);
		glVertex3d(0,0,5);
		glVertex3d(0,0,0);
		glVertex3d(5,0,0);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3d(0,0,0);
		glVertex3d(0,0,-5);
		glVertex3d(5,0,-5);
		glVertex3d(5,0,0);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3d(-5,0,5);
		glVertex3d(-5,0,0);
		glVertex3d(0,0,0);
		glVertex3d(0,0,5);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3d(-5,0,0);
		glVertex3d(-5,0,-5);
		glVertex3d(0,0,-5);
		glVertex3d(0,0,0);
	glEnd();
	glPopMatrix();
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
		vecColor[0] = 0.30;vecColor[1] = 0.40;vecColor[2] = 0;
		glMaterialfv(GL_FRONT,GL_AMBIENT, vecColor);
		vecColor[0] = 1;vecColor[1] = 1;vecColor[2] = 0;
		glMaterialfv(GL_FRONT,GL_DIFFUSE, vecColor);
		glMaterialfv(GL_FRONT,GL_SPECULAR, vecColor);
		glMateriali(GL_FRONT,GL_SHININESS, 64);
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
	glPopMatrix();
}

void refresh(void) {
	if (modeFP) firstPersonView();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	//glPushMatrix();

		if (modeLight1) {
			vecPos[0] = 0;vecPos[1] = 0;vecPos[2] = 0;vecPos[3] = 1;
			glLightfv(GL_LIGHT1,GL_POSITION,vecPos);
			vecColor[0] = 0.5;vecColor[1] = 0.5;vecColor[2] = 0.5;
			glLightfv(GL_LIGHT1,GL_AMBIENT ,vecColor);
			vecColor[0] = 0.75;vecColor[1] = 0.75;vecColor[2] = 0.75;
			glLightfv(GL_LIGHT1,GL_DIFFUSE,vecColor);
			vecColor[0] = 1;vecColor[1] = 1;vecColor[2] = 1;
			glLightfv(GL_LIGHT1,GL_SPECULAR,vecColor);
		}

		if (!modeFP) {
			glTranslatef(0,0,-cam.d);
			glRotatef(cam.theta,1,0,0);
			glRotatef(cam.phi,0,1,0);
			glTranslatef(-cam.VRP.x,-cam.VRP.y,-cam.VRP.z);
		}

		//glPushMatrix();
			if (modeLight0) {
				if (lightVertex == 0) {
					vecPos[0] = 5; vecPos[1] = 5; vecPos[2] = 5; vecPos[3] = 1;
				}
				else if (lightVertex == 1) {
					vecPos[0] = 5; vecPos[1] = 5; vecPos[2] = -5; vecPos[3] = 1;
				}
				else if (lightVertex == 2) {
					vecPos[0] = -5; vecPos[1] = 5; vecPos[2] = -5; vecPos[3] = 1;
				}
				else if (lightVertex == 3) {
					vecPos[0] = -5; vecPos[1] = 5; vecPos[2] = 5; vecPos[3] = 1;
				}
				vecColor[0] = 1;vecColor[1] = 1;vecColor[2] = 0;
				glLightfv(GL_LIGHT0,GL_AMBIENT ,vecColor);
				vecColor[0] = 0.75;vecColor[1] = 0.75;vecColor[2] = 0;
				glLightfv(GL_LIGHT0,GL_DIFFUSE,vecColor);
				vecColor[0] = 1;vecColor[1] = 1;vecColor[2] = 0;
				glLightfv(GL_LIGHT0,GL_SPECULAR,vecColor);
				glLightfv(GL_LIGHT0,GL_POSITION,vecPos);
			}
		//glPopMatrix();

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
			glTranslatef(coche.cen.x,0,coche.cen.z);
			glRotatef(coche.ang,0,1,0);
			pinta_model();
			vecPos[0] = coche.cen.x; vecPos[1] = 5; vecPos[2] = coche.cen.z; vecPos[3] = 1;
			if (modeLight2){
				vecColor[0] = 1;vecColor[1] = 1;vecColor[2] = 1;
				glLightfv(GL_LIGHT2,GL_POSITION,vecPos);
				glLightfv(GL_LIGHT2,GL_AMBIENT ,vecColor);
				glLightfv(GL_LIGHT2,GL_DIFFUSE,vecColor);
				glLightfv(GL_LIGHT2,GL_SPECULAR,vecColor);
			}
		glPopMatrix();
		
		glPushMatrix();
			pinta_terra();
		glPopMatrix();

	//glPopMatrix();

	glutSwapBuffers();
}

int main(int argc,const char * argv[])
{
	initGL(argc,argv);
	//m.load("porsche.obj");
	m.load("f-16.obj");
	slices = 50;
	stacks = 50;
	escena = esfera_cont_esce();
	//POS
	coche.cen.x = 0;
	coche.cen.y = 0;
	coche.cen.z = 0;
	coche.ang = 0;
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