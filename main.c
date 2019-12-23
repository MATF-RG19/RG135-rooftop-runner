#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "biblioteka.h"

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    srand(time(0));

    glutInitWindowSize(1000, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
   
    glClearColor(0.75, 0.75, 0.75, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);
    
    delta_z = 0.1;
    skok = 0;
    yCica = VISINAZGRADE;
    uSkoku = 0;
    duzinaSkoka = 1;
    pad = 0;
    pozicijaCice = prvaPozicija;
    trenutnaZKoordinata = prvaPozicija;
    zKoordinataPreSkoka = pozicijaCice;

	timer0Active = 0;
    timer1Active = 0;
    timer2Active = 0;
    timer3Active = 0;

    nadjiRazdaljine();
    glutMainLoop();

    return 0;
}


static void on_timer(int value){
    //TIMER JE ONAJ KOJI MI POMERA POZICIJU KAMERE 
    if (value == TIMER0 && !timer2Active){
		trenutnaZKoordinata += delta_z;
        pozicijaCice += delta_z;
		glutPostRedisplay();
    if (timer0Active && trenutnaZKoordinata<poslednjaZ-2*prvaPozicija)
		    glutTimerFunc(150, on_timer, TIMER0);
	}

    //TIMER ZA KRACI SKOK
    if (value == TIMER1){
        uSkoku = 1;
        skok+=0.1;
        if(skok > pi){
            skok = 0;
            timer1Active = 0;
            uSkoku = 0;
        }
        glutPostRedisplay();
        if (timer1Active && skok < pi)
		    glutTimerFunc(25, on_timer, TIMER1);
	}

    //ZA PAD
    if (value == TIMER2){
        timer0Active = 0;
        pad += 1;
        glutPostRedisplay();
        if (timer2Active && pad < 2*VISINAZGRADE)
		    glutTimerFunc(25, on_timer, TIMER2);
        else{
            sleep(2);
            exit(0);
        }

        
        //exit(1);
    }

    //ZA DUZI SKOK
    if (value == TIMER3){
        uSkoku = 1;
        skok+=0.1;
        //pozicijaCice+=1;
        if(skok > pi/2){
            skok = 0;
            timer3Active = 0;
            uSkoku = 0;
        }
        glutPostRedisplay();
        if (timer3Active && skok < pi/2)
		    glutTimerFunc(25, on_timer, TIMER3);
	}

}

static void on_keyboard(unsigned char key, int x, int y){
    switch (key) {
    case 27:
        exit(0);
        break;
    //KRENI   
	case 'g':
        if (!timer0Active) {
            glutTimerFunc(100, on_timer, TIMER0);
            timer0Active = 1;
        }
        break;
	//STANI		
	case 's':
        timer0Active = 0;
        break;
    
    //MANJI SKOK
    case 'j':
        duzinaSkoka = 1;
        zKoordinataPreSkoka = pozicijaCice;
        if (!timer1Active && !uSkoku) {
            glutTimerFunc(100, on_timer, TIMER1);
            timer1Active = 1;
        }
        break;
    //VECI SKOK
     case 'k':
        duzinaSkoka = 2;
        zKoordinataPreSkoka = pozicijaCice;
        if (!timer3Active && !uSkoku) {
            glutTimerFunc(100, on_timer, TIMER3);
            timer3Active = 1;
        }
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
            1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(-5, 2*VISINAZGRADE, 2+trenutnaZKoordinata-1,
              0, 2*VISINAZGRADE, trenutnaZKoordinata + VELICINAZGRADE,
              0, 1, 0);

    //printf("Gledam iz: %d,%d,%.2f\nGledam u: %d,%d,%.2f\n\n",-5,2*VISINAZGRADE,2+trenutnaZKoordinata,0,2*VISINAZGRADE,trenutnaZKoordinata+VELICINAZGRADE);
    
    glPushMatrix();
        namestiOsvetljenjeZgrada();
        iscrtajOse();
        iscrtajZgrade();
        iscrtajKraj();
    glPopMatrix();


    glPushMatrix();
        glTranslatef(0, 2*VISINAZGRADE-1.5+sin(duzinaSkoka*skok)-pad, pozicijaCice+prvaPozicija);
        glScalef(0.2,0.2,0.2);
        drawCica();
    glPopMatrix();
    printf("nalazi se %f\n",pozicijaCice+prvaPozicija);
    yCica = 2*VISINAZGRADE+sin(duzinaSkoka*skok)-pad-1.5;
    int pozicijaPrvePrethodne = pronadjiJednakiIliManji(0,2*BROJZGRADA,pozicijaCice+prvaPozicija);
    if((pozicijaPrvePrethodne%2 != 0 && !uSkoku)||(uSkoku && yCica < VISINAZGRADE)){
        printf("PADAM\nnalazi se %f\tprva bliza:%d\n",pozicijaCice+prvaPozicija,pozicijeIvicaZgrada[pozicijaPrvePrethodne]);
        timer0Active = 0;
        if(pozicijaPrvePrethodne%2 != 0 && !uSkoku)
            printf("prvi razlog\n\n");
        else
            printf("drugi razlog\n\n");
        if (!timer2Active) {
            glutTimerFunc(100, on_timer, TIMER2);
            timer2Active = 1;
            timer0Active = 0;
        }
    }

    glutSwapBuffers();
}

void nadjiRazdaljine(){
    int i = 0;
    int randR = 0;
    razdaljine[0] = 0;
    for(i = 1;i<BROJZGRADA;i++){
        randR = rand()%2;
        if(randR == 0){
            razdaljine[i] = MANJARAZDALJINA;
            sumaRazdaljina+=MANJARAZDALJINA;
        }
        else{
            razdaljine[i] = VECARAZDALJINA;
            sumaRazdaljina+=VECARAZDALJINA;
        }
        //printf("%d\n",razdaljine[i]);
    }
    poslednjaZ = (BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1;
    prvaPozicija = razdaljine[0];
    printf("prva pozicija: %d\n%f\n",razdaljine[0],prvaPozicija);
}

void iscrtajZgrade(){
    int i;
    int prethodnaDaljaStranica = 0;
    int trenutnaBlizaStranica = 0;
    for(i = 0;i<BROJZGRADA;i++){
        trenutnaBlizaStranica = prethodnaDaljaStranica + razdaljine[i];
        glPushMatrix();
        glScalef(1, VISINAZGRADE, 1);
        glTranslatef(0, 0, trenutnaBlizaStranica);
        glutSolidCube(VELICINAZGRADE);
        prethodnaDaljaStranica = trenutnaBlizaStranica + VELICINAZGRADE; 
        glPopMatrix();
        pozicijeIvicaZgrada[2*i] = trenutnaBlizaStranica;
        pozicijeIvicaZgrada[2*i+1] = trenutnaBlizaStranica + VELICINAZGRADE;
        printf("(%d, %d)\n",trenutnaBlizaStranica,trenutnaBlizaStranica+VELICINAZGRADE);
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
    GLfloat ambient_coeffs[] = { 0.0, 1.0, 0.0, 1 };
    GLfloat diffuse_coeffs[] = { 0.0, 1.0, 0.0, 1 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glBegin(GL_POLYGON);
        glVertex3f(-100,100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
        glVertex3f(100,100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
        glVertex3f(100,-100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
        glVertex3f(-100,-100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
    glEnd();
}

void namestiOsvetljenjeZgrada(){
    GLfloat light_position[] = { 12, 30, trenutnaZKoordinata, 0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1 };
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    
    GLfloat ambient_coeffs[] = { 0.0, 0.0, 1.0, 0.5 };
    GLfloat diffuse_coeffs[] = { 0.0, 0.0, 1.0, 0.5 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
}


void drawCica(){
    GLfloat ambient_coeffs[] = { 1.0, 0, 1.0, 0.5 };
    GLfloat diffuse_coeffs[] = { 1.0, 0, 1.0, 0.5 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    //glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    /*desna noga*/
     glPushMatrix();
        glColor3f(0, 0, 1);
        
        glRotatef(20*sin(-trenutnaZKoordinata), 1, 0, 0 );
        
        glTranslatef(0.75, -2.5, 0);
        glScalef(1, 4, 2);
        glutSolidCube(.5);
    glPopMatrix();
    
    /*leva ruka*/
     glPushMatrix();
        glColor3f(1, 0, 0);
        
        glTranslatef(0, 3.5, 0);
        glRotatef(20*sin(-trenutnaZKoordinata), 1, 0, 0 );
        glTranslatef(0, -3.5, 0);
        
        glTranslatef(-1.25, 0.75, 0);
        glScalef(1, 3, 2);
        glutSolidCube(.5);
    glPopMatrix();
    
     /*leva noga*/
     glPushMatrix();
        glColor3f(0, 0, 1);
        
        glRotatef(20*sin(trenutnaZKoordinata), 1, 0, 0 );
        
        glTranslatef(-0.75, -2.5, 0);
        glScalef(1, 4, 2);
        glutSolidCube(.5);
    glPopMatrix();
    
    /*desna ruka*/
     glPushMatrix();
        glColor3f(1, 0, 0);
        
        glTranslatef(0, 3.5, 0);
        glRotatef(20*sin(trenutnaZKoordinata), 1, 0, 0 );
        glTranslatef(0, -3.5, 0);
        
        glTranslatef(1.25, 0.75, 0);
        glScalef(1, 3, 2);
        glutSolidCube(.5);
    glPopMatrix();
    
    /*glava*/
    glPushMatrix();
        glColor3f(.7, .7, .3);
        glTranslatef(0, 2, 0);
        glutSolidSphere(.6, 20, 20);
    glPopMatrix();
    
    /*telo*/
    glPushMatrix();
        glColor3f(1, 0, 0);
        glScalef(2, 3, 1);
        glutSolidCube(1);
    glPopMatrix();
}

int pronadjiJednakiIliManji(int l, int d, float z){
    int i = 0;
    for(i=0; i<d; i++){
        if(pozicijeIvicaZgrada[i] > z){
            return i-1;
        }
    }
    return -1;
}
