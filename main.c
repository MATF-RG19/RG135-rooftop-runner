#include <stdlib.h>
#include <GL/glut.h>
#include<math.h>
#include<time.h>
#include<stdio.h>

#define VELICINAZGRADE 3
#define BROJZGRADA 50
#define MANJARAZDALJINA 3
#define VECARAZDALJINA 6
#define TIMER0 0

static int window_width, window_height;

#define pi M_PI

static float theta;

static float delta_phi, delta_theta;

int timer0Active;
int trenutnaZKoordinata;
int sumaRazdaljina;
int poslednjaZ;

static void on_timer(int value);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
void nadjiRazdaljine();
void iscrtajOse();
void iscrtajZgrade();
void iscrtajKraj();

int razdaljine[BROJZGRADA];

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    srand(time(0));

    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
   
    glClearColor(0.75, 0.75, 0.75, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);
    
    theta = 0.0001;
    delta_phi = delta_theta = pi / 90;
    
    trenutnaZKoordinata = 2;

	timer0Active = 0;

    nadjiRazdaljine();
    glutMainLoop();

    return 0;
}


static void on_timer(int value){
    //TIMER JE ONAJ KOJI MI POMERA POZICIJU KAMERE 
    if (value == TIMER0){
		trenutnaZKoordinata += 1;
		glutPostRedisplay();
    if (timer0Active && trenutnaZKoordinata<poslednjaZ-5)
		    glutTimerFunc(100, on_timer, TIMER0);
	}
}

static void on_keyboard(unsigned char key, int x, int y){
    switch (key) {
    case 27:
        exit(0);
        break;
        
	case 'g':
        if (!timer0Active) {
            glutTimerFunc(100, on_timer, TIMER0);
            timer0Active = 1;
        }
        break;
			
	case 's':
        timer0Active = 0;
        break;
    
 	

    case 'q':
        theta += delta_theta;
        if(theta > pi/2)
            theta -= delta_theta;
        glutPostRedisplay();
        break;
    case 'e':
        theta -= delta_theta;
        if(theta < -pi/2)
            theta += delta_theta;
        glutPostRedisplay();
        break;
    
    }
}

static void on_reshape(int width, int height){
    window_width = width;
    window_height = height;
}

static void on_display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
            80,
            window_width/(float)window_height,
            1, 30);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
             
    gluLookAt(trenutnaZKoordinata*sin(theta),12,trenutnaZKoordinata*cos(theta),
            0,10,trenutnaZKoordinata + 2*VELICINAZGRADE,
            0,1,0);
        
     
    iscrtajOse();
    iscrtajZgrade();
    iscrtajKraj();

    glutSwapBuffers();
}

void nadjiRazdaljine(){
    int i = 0;
    int randR = 0;
    for(i = 0;i<BROJZGRADA;i++){
        randR = rand()%2;
        if(randR == 0){
            razdaljine[i] = MANJARAZDALJINA;
            sumaRazdaljina+=MANJARAZDALJINA;
        }
        else{
            razdaljine[i] = VECARAZDALJINA;
            sumaRazdaljina+=VECARAZDALJINA;
        }
    }
    poslednjaZ = (BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1;
}

void iscrtajZgrade(){
    int i;
    int prethodnaDaljaStranica = razdaljine[0]+VELICINAZGRADE;
    int trenutnaBlizaStranica = razdaljine[0];
    for(i = 0;i<BROJZGRADA;i++){
        trenutnaBlizaStranica = prethodnaDaljaStranica + razdaljine[i];
        glPushMatrix();
        glColor3f(1,0, 1);
        glScalef(1, 5, 1);
        glTranslatef(0, 0, trenutnaBlizaStranica);
        glutWireCube(VELICINAZGRADE);
        prethodnaDaljaStranica = trenutnaBlizaStranica + VELICINAZGRADE; 
        glPopMatrix();
    }
}

void iscrtajOse(){
    glBegin(GL_LINES);
    	glColor3f(0,0,1);
    	//z - osa plava
    	glVertex3f(0,0,0);
    	glVertex3f(0,0,100);
    glEnd();
    glBegin(GL_LINES);
    	glColor3f(0,1,0);
    	//y - osa zelena
    	glVertex3f(0,0,0);
    	glVertex3f(0,100,0);
    glEnd();
    glBegin(GL_LINES);
    	glColor3f(1,0,0);
    	//x - osa crvena
    	glVertex3f(0,0,0);
    	glVertex3f(100,0,0);
    glEnd();
}

void iscrtajKraj(){
    
    glBegin(GL_POLYGON);
        glColor3f(0,15,0);
        glVertex3f(-100,100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
        glVertex3f(100,100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
        glVertex3f(100,-100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
        glVertex3f(-100,-100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
    glEnd();
}


