#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "image.h"

#define VELICINAZGRADE 3.0
#define VISINAZGRADE 5
#define BROJZGRADA 15
#define MANJARAZDALJINA 3
#define VECARAZDALJINA 6

#define FILENAME0 "textures/zgrada1.bmp"
#define FILENAME1 "textures/nebo.bmp"
#define FILENAME2 "textures/pod1.bmp"

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
void nacrtajZgradu();
void drawCica();
void nacrtajNebo();
void nacrtajPod();

/* Identifikatori tekstura. */
static GLuint names[4];

/* Funkcija initalize() vrsi OpenGL inicijalizaciju. */
static void initialize(void);

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
    
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);

    nadjiRazdaljine();
    initialize();

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

    GLfloat light_position[] = { -5, 2*VISINAZGRADE+2.5, 2+trenutnaZKoordinata-1, 0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1 };    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
            80,
            window_width/(float)window_height,
            1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(-5, 2*VISINAZGRADE+2.5, 2+trenutnaZKoordinata-1,
              0, 2*VISINAZGRADE, trenutnaZKoordinata + VELICINAZGRADE,
              0, 1, 0);

    glPushMatrix();
        glTranslatef(0, 2 * VISINAZGRADE +1 + sin(pi * parametar_skoka) - pad, trenutnaZKoordinata + 0.2);
        glScalef(0.2,0.2,0.2);
        drawCica();
    glPopMatrix();

    glPushMatrix();
        iscrtajZgrade();
    glPopMatrix();

    
    glPushMatrix();
        nacrtajNebo();
    glPopMatrix();

    glPushMatrix();
        nacrtajPod();
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
        //glScalef(1, VISINAZGRADE, 1);
        glTranslatef(0, 0, trenutnaBlizaStranica);
        //glutSolidCube(VELICINAZGRADE);
        nacrtajZgradu();
        prethodnaDaljaStranica = trenutnaBlizaStranica + VELICINAZGRADE; 
        glPopMatrix();
        pozicijeIvicaZgrada[2*i] = trenutnaBlizaStranica;
        pozicijeIvicaZgrada[2*i+1] = trenutnaBlizaStranica + VELICINAZGRADE;
    }
}

void iscrtajKraj(){
    glPushMatrix();
        GLfloat ambient_coeffs[] = { 0.0, 1.0, 0.0, 1 };
        GLfloat diffuse_coeffs[] = { 0.0, 1.0, 0.0, 1 };
        
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs);
        glBegin(GL_POLYGON);
            glVertex3f(-100,100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
            glVertex3f(100,100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
            glVertex3f(100,-100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
            glVertex3f(-100,-100,(BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1);
        glEnd();
    glPopMatrix();
}

void drawCica(){

    /*desna noga*/
     glPushMatrix();
        GLfloat ambient_coeffs1[] = { 29/255.0, 47/255.0, 119/255.0, 1 };
        GLfloat diffuse_coeffs1[] = { 29/255.0, 47/255.0, 119/255.0, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs1);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs1);
        
        glRotatef(20*sin(-trenutnaZKoordinata), 1, 0, 0 );
        
        glTranslatef(0.75, -2.5, 0);
        glScalef(1, 4, 2);
        glutSolidCube(.5);
    glPopMatrix();
    
    /*leva ruka*/
     glPushMatrix();
        GLfloat ambient_coeffs2[] = { 213/255.0, 168/255.0, 77/255.0, 1 };
        GLfloat diffuse_coeffs2[] = { 213/255.0, 168/255.0, 77/255.0, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs2);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs2);
        glTranslatef(0, 3.5, 0);
        glRotatef(20*sin(-trenutnaZKoordinata), 1, 0, 0 );
        glTranslatef(0, -3.5, 0);
        
        glTranslatef(-1.25, 0.75, 0);
        glScalef(1, 3, 2);
        glutSolidCube(.5);
    glPopMatrix();
    
     /*leva noga*/
     glPushMatrix();
        GLfloat ambient_coeffs3[] = { 29/255.0, 47/255.0, 119/255.0, 1 };
        GLfloat diffuse_coeffs3[] = { 29/255.0, 47/255.0, 119/255.0, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs3);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs3);
        glRotatef(20*sin(trenutnaZKoordinata), 1, 0, 0 );
        
        glTranslatef(-0.75, -2.5, 0);
        glScalef(1, 4, 2);
        glutSolidCube(.5);
    glPopMatrix();
    
    /*desna ruka*/
     glPushMatrix();
        GLfloat ambient_coeffs4[] = { 213/255.0, 168/255.0, 77/255.0, 1 };
        GLfloat diffuse_coeffs4[] = { 213/255.0, 168/255.0, 77/255.0, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs4);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs4);
        glTranslatef(0, 3.5, 0);
        glRotatef(20*sin(trenutnaZKoordinata), 1, 0, 0 );
        glTranslatef(0, -3.5, 0);
        
        glTranslatef(1.25, 0.75, 0);
        glScalef(1, 3, 2);
        glutSolidCube(.5);
    glPopMatrix();
    
    /*glava*/
    glPushMatrix();
        GLfloat ambient_coeffs5[] = { 229/255.0, 180/255.0, 94/255.0, 1 };
        GLfloat diffuse_coeffs5[] = { 229/255.0, 180/255.0, 94/255.0, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs5);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs5);
        glColor3f(.7, .7, .3);
        glTranslatef(0, 2, 0);
        glutSolidSphere(.6, 20, 20);
    glPopMatrix();
    
    /*telo*/
    glPushMatrix();
        GLfloat ambient_coeffs6[] = { 213/255.0, 168/255.0, 77/255.0, 1 };
        GLfloat diffuse_coeffs6[] = { 213/255.0, 168/255.0, 77/255.0, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs6);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs6);
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

void nacrtajZgradu(){
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glBegin(GL_QUADS);
    
    
        // Zapadna strana
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3f(-VELICINAZGRADE/2.0, 0, 0);
        glTexCoord2f(0.7, 0);
        glVertex3f(VELICINAZGRADE/2.0, 0, 0);
        glTexCoord2f(0.7, 0.5);
        glVertex3f(VELICINAZGRADE/2.0, 2*VISINAZGRADE, 0);
        glTexCoord2f(0, 0.5);
        glVertex3f(-VELICINAZGRADE/2.0, 2*VISINAZGRADE, 0);

        // Gornja strana
        glNormal3f(0, 1, 0);
        glTexCoord2f(0.8, 0);
        glVertex3f(-VELICINAZGRADE/2.0,2*VISINAZGRADE , 0);
        glTexCoord2f(1, 0);
        glVertex3f(VELICINAZGRADE/2.0,2*VISINAZGRADE , 0);
        glTexCoord2f(1, 1);
        glVertex3f(VELICINAZGRADE/2.0,2*VISINAZGRADE , VELICINAZGRADE);
        glTexCoord2f(0.8, 1);
        glVertex3f(-VELICINAZGRADE/2.0,2*VISINAZGRADE , VELICINAZGRADE);
        
        // Juzna strana
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-VELICINAZGRADE/2.0, 0, 0);
        glTexCoord2f(0.7, 0);
        glVertex3f(-VELICINAZGRADE/2.0, 0, VELICINAZGRADE);
        glTexCoord2f(0.7, 0.5);
        glVertex3f(-VELICINAZGRADE/2.0, 2*VISINAZGRADE, VELICINAZGRADE);
        glTexCoord2f(0, 0.5);
        glVertex3f(-VELICINAZGRADE/2.0, 2*VISINAZGRADE, 0);

        // Istocna strana
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3f(-VELICINAZGRADE/2.0, 0, VELICINAZGRADE);
        glTexCoord2f(0.7, 0);
        glVertex3f(VELICINAZGRADE/2.0, 0, VELICINAZGRADE);
        glTexCoord2f(0.7, 0.5);
        glVertex3f(VELICINAZGRADE/2.0, 2*VISINAZGRADE, VELICINAZGRADE);
        glTexCoord2f(0, 0.5);
        glVertex3f(-VELICINAZGRADE/2.0, 2*VISINAZGRADE, VELICINAZGRADE);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

}

static void initialize(void){
    /* Objekat koji predstavlja teskturu ucitanu iz fajla. */
    Image * image;
    glEnable(GL_DEPTH_TEST);

    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

    /*INICIJALIZACIJA*/
    image = image_init(0, 0);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(4, names);


    /*TEKSTURA ZGRADE*/
    image_read(image, FILENAME0);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /*TEKSTURA NEBA*/
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /*TEKSTURA PODA*/
    image_read(image, FILENAME2);

    glBindTexture(GL_TEXTURE_2D, names[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    
    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);
}

void nacrtajNebo(){
    glBindTexture(GL_TEXTURE_2D, names[2]);
    glBegin(GL_QUADS);

        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3f(20, 0, -20);
        glTexCoord2f(1, 0);
        glVertex3f(20, 0, ((BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1)+100);
        glTexCoord2f(1, 1);
        glVertex3f(20, 30, ((BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1)+100);
        glTexCoord2f(0, 1);
        glVertex3f(20, 30,-20);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void nacrtajPod(){
    glBindTexture(GL_TEXTURE_2D, names[3]);
    glBegin(GL_QUADS);

        glNormal3f(0, 100, 0);
        glTexCoord2f(0, 20);
        glVertex3f(20, 0, -20);
        glTexCoord2f(100, 100);
        glVertex3f(20, 0, ((BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1)+100);
        glTexCoord2f(0, 100);
        glVertex3f(-20, 0, ((BROJZGRADA+1)*VELICINAZGRADE + sumaRazdaljina+1)+100);
        glTexCoord2f(0, 0);
        glVertex3f(-20, 0,-20);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}