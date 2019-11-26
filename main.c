#include <stdlib.h>
#include <GL/glut.h>
#include<math.h>
#include<time.h>
#include<stdio.h>

#define TIMER0 0

static int window_width, window_height;

const static float pi = 3.141592653589793;

static float phi, theta;

static float delta_phi, delta_theta;

int timer0Active;
int pozicija;

static void on_timer(int value);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);


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
    
    phi = theta = 0;
    delta_phi = delta_theta = pi / 90;
    
    pozicija = -5;

	timer0Active = 0;

    glutMainLoop();

    return 0;
}


static void on_timer(int value){
    //TIMER JE ONAJ KOJI MI POMERA POZICIJU KAMERE 
    if (value == TIMER0){
		pozicija += 0.5;
		glutPostRedisplay();

		/* Po potrebi se ponovo postavlja tajmer. */
		if (timer0Active)
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
    
 	}   
}

static void on_reshape(int width, int height){
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
}

static void on_display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
            60,
            window_width/(float)window_height,
            1, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
             
     gluLookAt(
            0, 6, pozicija,
            0, 5, pozicija + 5,
            0, 0, 1
        );
        
     
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

//generisanje zgrada
    
    int preth = 0;
    for(int i = 0;i<100;i+=2){
    	//generisem razdaljinu
    	int r = rand() % 2;
    	//fprintf(stderr,"%d\n",r);
		glPushMatrix();
			glColor3f(10,0, 10);
			glScalef(1, 10, 1);
            glTranslatef(0, 0, preth + i);
            preth += r;
			glutWireCube(1);
		glPopMatrix();
    }

    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}
