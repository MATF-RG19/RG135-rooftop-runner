#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define VELICINAZGRADE 3.0
#define VISINAZGRADE 5
#define BROJZGRADA 15
#define MANJARAZDALJINA 3
#define VECARAZDALJINA 6


// `60` je za 60 fps
#define INTERVAL_AZURIRANJA (1000/60)

#define TIMER0 0

#define pi M_PI

static int window_width, window_height;
static float delta_z = 0.1f;

float trenutnaZKoordinata;
float sumaRazdaljina;
float poslednjaZ;
float prvaPozicija;
float pad = 0;

static void on_timer(int value);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
void nadjiRazdaljine();
void iscrtajOse();
void iscrtajZgrade();
void iscrtajKraj();
void namestiOsvetljenjeZgrada();
void drawCica();

int pronadjiJednakiIliManji(int l, int d,float z);

int proveri_da_li_pada();

int razdaljine[BROJZGRADA];
int pozicijeIvicaZgrada[BROJZGRADA*2];

typedef enum { HODA, SKACE, DUGACKO_SKACE, PADA } Stanje;

Stanje stanje = HODA;
int igra_u_toku = 0;
float parametar_skoka = 0;

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    srand(time(NULL));

    glutInitWindowSize(1000, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
   
    glClearColor(0.75, 0.75, 0.75, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);

    nadjiRazdaljine();

    glutTimerFunc(INTERVAL_AZURIRANJA, on_timer, TIMER0);

    glutMainLoop();

    return 0;
}


static void on_timer(int value){
    (void) (value); // Ovo se ovako piše jer se ne koristi parametar value

    if (!igra_u_toku) { // Ako igra nije u toku, samo ponovo okini tajmer za 50ms.
        glutTimerFunc(INTERVAL_AZURIRANJA, on_timer, TIMER0);
        return;
    }

    if (stanje != PADA) { // U svim slučajeva osim pri padanju, čiča se kreće unapred
        trenutnaZKoordinata += delta_z;
    }

    switch (stanje) {
        case HODA: {
            if (proveri_da_li_pada()) {
                stanje = PADA;
            }
        } break;

        case SKACE: {
            parametar_skoka+=0.02f;
            if(parametar_skoka > 1.0) {
                parametar_skoka = 0;
                stanje = HODA;
            }
        } break;
        case DUGACKO_SKACE: {
            parametar_skoka+=0.014f;
            if(parametar_skoka > 1.0) {
                parametar_skoka = 0;
                stanje = HODA;
            }
        } break;

        case PADA: {
            pad += 0.1f;
            if (pad >= 2 * VISINAZGRADE) {
                sleep(2);
                exit(0);
            }
        } break;

        default:
            break;
    }

    // KRAJ IGRE
    if (trenutnaZKoordinata >= poslednjaZ) {
        exit(0);
    }

    glutPostRedisplay();
    glutTimerFunc(INTERVAL_AZURIRANJA, on_timer, TIMER0);
}

static void on_keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 27:
            exit(0);
        //KRENI
        case 'g': {
            igra_u_toku = 1;
        }         break;

        //STANI
        case 's':
            igra_u_toku = 0;
            break;
    
        //MANJI SKOK
        case 'j':
            if (stanje == HODA) {
                stanje = SKACE;
            }
            break;
        //VECI SKOK
         case 'k':;
            if (stanje == HODA) {
                stanje = DUGACKO_SKACE;
            }
            break;
        default:
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

    glPushMatrix();
        namestiOsvetljenjeZgrada();
        iscrtajOse();
        iscrtajZgrade();
        iscrtajKraj();
    glPopMatrix();


    glPushMatrix();
        glTranslatef(0, 2 * VISINAZGRADE - 1.5 + sin(pi * parametar_skoka) - pad, trenutnaZKoordinata + 0.2);
        glScalef(0.2,0.2,0.2);
        drawCica();
    glPopMatrix();


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
    }
    poslednjaZ = (BROJZGRADA)*VELICINAZGRADE + sumaRazdaljina;
    prvaPozicija = razdaljine[0];
}

void iscrtajZgrade(){
    int i;
    int prethodnaDaljaStranica = 0;
    int trenutnaBlizaStranica = 0;
    for(i = 0;i<BROJZGRADA;i++){
        trenutnaBlizaStranica = prethodnaDaljaStranica + razdaljine[i];
        glPushMatrix();
        glScalef(1, VISINAZGRADE, 1);
        glTranslatef(0, 0, trenutnaBlizaStranica + VELICINAZGRADE/2);
        glutSolidCube(VELICINAZGRADE);
        prethodnaDaljaStranica = trenutnaBlizaStranica + VELICINAZGRADE; 
        glPopMatrix();
        pozicijeIvicaZgrada[2*i] = trenutnaBlizaStranica;
        pozicijeIvicaZgrada[2*i+1] = trenutnaBlizaStranica + VELICINAZGRADE;
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
    for (int i = 0; i < d; i++) {
        if (pozicijeIvicaZgrada[i] > z) {
            return i - 1;
        }
    }
    return -1;
}

int proveri_da_li_pada() {
    int pozicijaPrvePrethodne = pronadjiJednakiIliManji(0,2*BROJZGRADA,trenutnaZKoordinata);
    return pozicijaPrvePrethodne % 2 != 0;
}
