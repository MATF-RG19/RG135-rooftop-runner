#define VELICINAZGRADE 3
#define VISINAZGRADE 5
#define BROJZGRADA 50
#define MANJARAZDALJINA 3
#define VECARAZDALJINA 6

#define TIMER0 0
#define TIMER1 1
#define TIMER2 2
#define TIMER3 3

#define pi M_PI

static int window_width, window_height;
static float delta_z;

int timer0Active;
int timer1Active;
int timer2Active;
int timer3Active;

float trenutnaZKoordinata;
float pozicijaCice;
float sumaRazdaljina;
float poslednjaZ;
float prvaPozicija;
float skok;
int uSkoku;
float pad;
float zKoordinataPreSkoka;
float yCica;

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

int razdaljine[BROJZGRADA];
int pozicijeIvicaZgrada[BROJZGRADA*2];

int duzinaSkoka;