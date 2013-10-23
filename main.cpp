#if defined(__APPLE__)
        #include <OpenGL/OpenGL.h>
        #include <GLUT/GLUT.h>
#else
        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/freeglut.h>
#endif
#include "iostream"
#include "model.h"
#include"math.h" //double asin(double x);
#define _USE_MATH_DEFINES
using namespace std;


Model m;
int alt, amp; // de la finestra
int grausX, grausY;
float despX, despY; // despX de la rotacio
char estat;
bool paretsVisibles;
double relacioAspecte;
double radiEsferaMinima;
double dist;
bool ortogonal;

struct caixaMinima{
    float minX, minY, minZ, maxX, maxY, maxZ;
};

caixaMinima cm;

void inicialitzaCaixaMinima(){

    //EM RECORRO EL VECTOR DE VERTEX A SACO, perque es especific de cada model,
    // el recorres tot sabent que estan les x, y i z en ordre
    cm.maxX = cm.minX = m.vertices()[0];
    cm.maxY = cm.minY = m.vertices()[1];
    cm.maxZ = cm.minZ = m.vertices()[2];

    for( int i = 3; i < m.vertices().size(); i+=3 ){ // vector de faces
            float xProvisional = m.vertices()[ i ]; // x
            float yProvisional = m.vertices()[ i+1]; // y
            float zProvisional = m.vertices()[ i+2]; // z

            if( xProvisional > cm.maxX ) cm.maxX = xProvisional;
            if( xProvisional < cm.minX ) cm.minX = xProvisional;
            if( yProvisional > cm.maxY ) cm.maxY = yProvisional;
            if( yProvisional < cm.minY ) cm.minY = yProvisional;
            if( zProvisional > cm.maxZ ) cm.maxZ = zProvisional;
            if( zProvisional < cm.minZ ) cm.minZ = zProvisional;
    }

    cout << "Caixa " << cm.maxX << " " << cm.minX << " " << cm.maxY << " " << cm.minY << " " << cm.maxZ << " " << cm.minZ << endl;
}

void inicialitzaEsferaMinima(){


    caixaMinima c;
    c.minX = c.minZ = -5.0;
    c.maxX = c.maxZ = 5.0;
    c.minY = 0.0;
    c.maxY = 1.5; //inicialita amb el terra

    double restaqx = pow((c.maxX-c.minX),2.0);
    double restaqy = pow((c.maxY-c.minY),2.0);
    double restaqz = pow((c.maxZ-c.minZ),2.0);
    double diametre = sqrt(restaqx + restaqy + restaqz);

    radiEsferaMinima = diametre/2 ;

    dist = 2*radiEsferaMinima;

    cout << " radiesf -> " << diametre << endl;

}

void escalaModel( float escalaX, float escalaY, float escalaZ ){
    // perque estigui proporcionat es devideix 1/mida maxima
    // sino el deforma perque fa que tots els costats vagin de -1 a 1
    float midaX = ( cm.maxX-cm.minX) ;
    float midaY = ( cm.maxY-cm.minY) ;
    float midaZ = ( cm.maxZ-cm.minZ) ;
    float midamax = max( midaX, midaY );
    midamax = max( midamax, midaZ );
    // 1/midamax fa la relacio a 1, i *2 perque vagi de -1 a 1
    // i * escala per fer la escala que volem * 0.5 per compensar el 2
    glScalef( escalaX*(2/midamax) , escalaY*(2/midamax) , escalaZ*(2/midamax) );
}

void desplacarAlCentre(){
    //desplaço en sentit negatiu el centre de la caixa
    float px = (cm.maxX+cm.minX)/(-2.0);
    //float py = (cm.maxY+cm.minY)/(-2.0); //CENTRE DE LES Y
    float py = cm.minY * -1.0; // per agafar el centre la base
    float pz = (cm.maxZ+cm.minZ)/(-2.0);
    glTranslatef( px, py , pz );
}

void pintaModel(){

    glPushMatrix();

    float movX = ((despX*2.0) -1.0);
    float movY = ((despY*(2.0))-1.0);

    glTranslatef( movX, 0.0 , movY ); // 0.25 es la distancia del centre als peus

    escalaModel( 0.5, 0.5, 0.5); // 2*0.25= 0.5 que demana l enunciat

    desplacarAlCentre();

    for(int i = 0; i < m.faces().size(); i++){
       glColor4fv(  &Materials[ m.faces()[i].mat ].diffuse[0]);

        glBegin(GL_TRIANGLES);
            glVertex3dv( &m.vertices()[m.faces()[i].v[0]]) ;
            glVertex3dv( &m.vertices()[m.faces()[i].v[1]]) ;
            glVertex3dv( &m.vertices()[m.faces()[i].v[2]]) ;
        glEnd();
    }
    glPopMatrix();
}

void pintaNinotNeu( int posX, int posY, int posZ ){

    glPushMatrix(); //cos
        glTranslatef(posX, 0.4, posZ);
        glColor3f(1.0, 1.0, 1.0);
        glutSolidSphere(0.4, 30, 30);
    glPopMatrix();
    glPushMatrix(); // cap
        glTranslatef(posX, 0.9, posZ);
        glutSolidSphere(0.2, 30, 30);
        glPopMatrix();
    glPushMatrix(); // nas
        glTranslatef(posX, 0.9, posZ);
        glColor3f(1.0, 0.4, 0.0);
        glutSolidCone(0.075, 0.2, 20 , 20);
    glPopMatrix();
}

void pintaTerra(){

    glPushMatrix();
    glColor3f( 0.55, 0.24, 0.2);
    glBegin( GL_QUADS );
        glVertex3f( 5.0, 0.0, 5.0 );
        glVertex3f( 5.0, 0.0, -5.0 );
        glVertex3f( -5.0, 0.0, -5.0 );
        glVertex3f( -5.0, 0.0, 5.0 );
    glEnd();
    glPopMatrix();
}

void pintaParets(){
    glPushMatrix();
    glColor3f(0.0, 0.6, 0.0);
    glBegin( GL_QUADS );
        glVertex3f( 0.5, 0.0, -1.5 );
        glVertex3f( 4.5, 0.0, -1.5 );
        glVertex3f( 4.5, 1.5, -1.5 );
        glVertex3f( 0.5, 1.5, -1.5 );
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.6, 0.0);
    glBegin( GL_QUADS );
    glVertex3f( -4.9, 0.0, 5.0 );
    glVertex3f( -4.9, 0.0, -5.0 );
    glVertex3f( -4.9, 1.5, -5.0 );
    glVertex3f( -4.9, 1.5, 5.0 );
    glEnd();
    glPopMatrix();
}

void pintaEsfera(){
    glPushMatrix(); //cos
        glColor3f(0.0, 0.5, 0.5);
        glutWireSphere( radiEsferaMinima , 30, 30);
    glPopMatrix();
}

void refresh(void) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        pintaEsfera();

        pintaTerra();

        pintaModel();

        pintaNinotNeu( 2.5, 0, 2.5);
        pintaNinotNeu( -2.5, 0, 2.5);
        pintaNinotNeu( -2.5, 0, -2.5);
        pintaNinotNeu( 2.5, 0, -2.5);

        if(paretsVisibles) pintaParets();

        glutSwapBuffers();
}


void motionf(int newx, int newy){
    // depenent de l'estat actualitza els graus o el desp
    if( estat == 'c'){
        despX = ( (float) newx / (float) amp );
        despY = ( (float) newy / (float) alt );
    }

    if(estat == 'm'){

        grausX = ((float) newx/ (float) amp )*360;
        grausY = ((float) newy/ (float) alt )*(-360);

        glRotated( grausY, 1.0, 0.0, 0.0); // x
        glRotated( grausX, 0.0, 1.0, 0.0); // y

    }
    glutPostRedisplay();
}


void posicionaCamera(){ //GlLookAt
   glLoadIdentity();
   double ox, oy, oz, vx, vy, vz, upx, upy, upz;

   ox =0;   oy =0;  oz = dist;
   vx =0;   vy =0;  vz = 0;
   upx =0;  upy =1; upz =0;

   gluLookAt(ox, oy, oz, vx, vy, vz, upx, upy, upz );
}

void initOrtogonal(){

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double migAmpleCamera = radiEsferaMinima; // del centre al canto
    double migAltCamera = radiEsferaMinima; // de lcentre a dalt

    if( relacioAspecte >= 1 ) // es mes ample que alt
        migAmpleCamera = migAmpleCamera * relacioAspecte;
    else    // es mes alt que ample
        migAltCamera = migAltCamera / relacioAspecte;

    glOrtho(
        -migAmpleCamera, migAmpleCamera, // abaix esquerra
        -migAltCamera, migAltCamera,     // adalt dreta
          dist-radiEsferaMinima, dist+radiEsferaMinima  // zNear i zFar just en la escena
        );

    glMatrixMode(  GL_MODELVIEW );

}

void initPerspectiva(){

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double migAmpleCamera = radiEsferaMinima; // del centre al canto
    double migAltCamera = radiEsferaMinima; // de lcentre a dalt

    double angle = asin( radiEsferaMinima / dist );
    
    //si es mes ample que alt o igual ----> "no cal fer res"

    if( relacioAspecte < 1.0 )  // es mes alt que ample
        angle = atan( tan(angle) / relacioAspecte  );

    angle = (angle*180)/M_PI; // pas a radiants

    gluPerspective(
          2*angle,
          relacioAspecte,
          dist-radiEsferaMinima , radiEsferaMinima+dist  // zNear i zFar just en la escena
        );

    glMatrixMode(  GL_MODELVIEW );

}


void initCamara(){

    if( ortogonal )
        initOrtogonal();

    else if( ! ortogonal )
        initPerspectiva();

    posicionaCamera();    //glLookAt

    glutPostRedisplay();
}

void reshape( int ampleW, int altW){

    cout << "new size: x= " << ampleW << ", y= " << altW << endl;

    // el que vull que tingui el viewPort (es pot posar directament la crida a viewport)
    amp = ampleW;
    alt = altW;

    relacioAspecte = (double) ampleW / (double) altW;
    cout << " relacio d' aspecte " << relacioAspecte << endl;

    initCamara();

    glViewport( 0, 0, ampleW, altW );

    glutPostRedisplay();
}

void kbrd( unsigned char key, int x, int y ){
    if( key == 'h' ){
        cout << endl << "###############################################################" << endl;
        cout << endl <<" *   f      -> omplir cares" << endl;
        cout << endl <<" *   l      -> veure contorn de les cares" << endl;
        cout << endl <<" *   ESC    -> tancar el programa" << endl;
        cout << endl <<" *   c      -> fer caminar al lego quan arrastres" << endl;
        cout << endl <<" *   e      -> fer girar l' escena amb angles d Euler" << endl;
        cout << endl <<" *   v      -> parets visibles / no visibles" << endl;
        cout << endl <<" *   any    -> desctivar els modes anteriors"<< endl;
        cout << endl <<"###############################################################" << endl << endl;
    }
    else if( key == 'f' ) {
        cout <<  endl << "  CARES" << endl;   estat = 'f';
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glutPostRedisplay();
    }
    else if(key == 'l'){
        cout <<  endl << "  LINIES" << endl;  estat = 'l';
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glutPostRedisplay();
    }
    else if( key == 'c'){
        cout << endl << "   CAMINAR" << endl; estat = 'c';
    }
    else if( key == 'e'){
        cout << endl << "   EULER" << endl; estat = 'e';
    }
    else if( key == 'v'){
        cout << endl << "   PARETS" << endl; estat = 'v';
        paretsVisibles = ! paretsVisibles;
        glutPostRedisplay();
    }
    else if( key == 'x'){
        cout <<  endl << "  CAMERA ORTOGONAL" << endl << endl; estat = 'x';
        ortogonal = true;
        initCamara();
    }
    else if( key == 'p'){
        cout << endl << "   CAMERA PERSPECTIVA" << endl << endl; estat = 'p';
        ortogonal = false;
        initCamara();
    }
    else if( key == 27) exit(0); // ESC

    else estat = 'n'; // estat nul
}

int main(int argc, const char * argv[]) {

        glutInit(&argc, (char**)argv );
        glutInitWindowPosition( 0, 0 );
        glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
        glutInitWindowSize(600, 600);
        glutCreateWindow("IDI:PractiquesOpenGL");
        glEnable(GL_DEPTH_TEST);

        estat = 'f';
        grausX = grausY = 0;
        despX = 0.5; despY = 0.5;
        ortogonal = paretsVisibles = true;

        cout << endl << "------------------- h  ->  per veure el menu --------------------- " << endl << endl;

        m.load(argv[1]); // l objecte que se li passa per parametre l programa

        inicialitzaCaixaMinima(); //del model
        inicialitzaEsferaMinima(); // de l'escena

        initCamara();

        glutDisplayFunc(refresh);
        glutReshapeFunc( reshape ); // resize
        glutMotionFunc( motionf ); //  drag
        glutKeyboardFunc( kbrd ); // teclat

        glutMainLoop();
        return 0;
}

/*

• Permeti inspeccionar l’escena modificant els angles d’Euler amb el ratolı.

• Permeti fer zoom modificant l’optica de la camera activa, tal i com s’indica en la Seccio 6.

• Permeti passar a mode d’inspeccio en primera persona e inspeccionar l’escena amb les dos tecniques de walk indicades a la Seccio 8.2.

• Permeti fer un reset per tornar a la visualitzacio de l’escena des de la camera inicial, en premer la tecla ’r’.

-------------------------------- FET --------------------------------------------------------------------------

• Hi hagi un help que mostri com activar les diferents funcionalitats.
• En iniciar l’aplicacio mostri l’escena indicada en la Seccio 7 des d’una posicio arbitraria (sense retallar),
    amb una camera perspectiva i aprofitant la grandaria del viewport (definit com tota la finestra grafica).
• Permeti fer visibles/invisibles les parets en qualsevol moment de l’execucio de l’aplicacio (tecla ’v’).
• Permeti communtar entre les cameres perspectiva i axonometrica (tecla ’p’). En cap cas ha d’haver-hi deformacio si es fa un resize’.

*/
