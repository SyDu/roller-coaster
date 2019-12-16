//
//  main.c
//  Asteroid
//
//  Created by mac on 2018-06-04.
//  Copyright © 2018 mac. All rights reserved.
//
/*
 this is Siyi Du B00750733 assignment 1
 2018-jun-9
 */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <GLUT/GLUT.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG 180.0/M_PI
#define DEG2RAD M_PI/180.0

#define myTranslate2D(x,y) glTranslated(x, y, 0.0)
#define myScale2D(x,y) glScalef(x, y, 1.0)
#define myRotate2D(angle) glRotatef(RAD2DEG*angle, 0.0, 0.0, 1.0)


#define MAX_PHOTONS    8
#define MAX_ASTEROIDS    7
#define MAX_VERTICES    16

/*
 define speed
 */
#define PhotonSPEED 0.002
#define AsteroidSpeed 0.0003
#define ShipSpeed 0.0006

#define drawCircle() glCallList(circle)


static GLuint    circle;
void
buildCircle() {
    GLint   i;
    
    circle = glGenLists(1);
    glNewList(circle, GL_COMPILE);
    glBegin(GL_POLYGON);
    for(i=0; i<40; i++)
        glVertex2d(cos(i*M_PI/20.0), sin(i*M_PI/20.0));
    glEnd();
    glEndList();
}

/* -- type definitions ------------------------------------------------------ */

typedef struct Coords {
    double        x, y;
} Coords;

typedef struct{
    double    x, y, phi, dx, dy;
    double cor[3];
} Ship;

typedef struct {
    int    active;
    double    x, y, dx, dy,ang;
} Photon;

typedef struct {
    int    active, nVertices;
    double    x, y, phi, dx, dy, dphi,size;
    Coords    coords[MAX_VERTICES];
} Asteroid;


/* -- function prototypes --------------------------------------------------- */

static void    myDisplay(void);
static void    myKey(unsigned char key, int x, int y);
static void    keyPress(int key, int x, int y);
static void    keyRelease(int key, int x, int y);
static void    myReshape(int w, int h);

static void    init(void);
static void    drawShip(Ship *s);
static void    drawPhoton(Photon *p);
static void    drawAsteroid(Asteroid *a);

static double myRandom(double min, double max);

static void myIdle();
static void myMouse(int button, int state, int x,int y);
static void getgrade();
static char* intToChar(int n);
static double transCoordX(double x,double y,double phi);
static double transCoordY(double x,double y,double phi);

/* -- global variables ------------------------------------------------------ */

static int    up=0, down=0, left=0, right=0,pau=0,dead=0,turn=0,gameov=0 ;    /* state of cursor keys */
static double    xMax, yMax;
static Ship    ship;
static Coords coord;
static Photon    photons[MAX_PHOTONS];
static Asteroid    asteroids[MAX_ASTEROIDS];
static int grade=0;
static double speedShip=ShipSpeed;


int main(int argc, const char * argv[]) {
    ship.cor[0]=1,ship.cor[1]=1,ship.cor[2]=1;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Asteroids");
    buildCircle();
    glutDisplayFunc(myDisplay);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(myKey);
    glutSpecialUpFunc(keyRelease);
    glutIdleFunc(myIdle);
    glutSpecialFunc(keyPress);
    
    glutReshapeFunc(myReshape);
    glutMouseFunc(myMouse);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    init();
    
    glutMainLoop();
    
    
    return 0;
}

/* -- callback functions ---------------------------------------------------- */
static void getgrade()
{
    char *c;
    glColor3f(1, 0, 1);
    
    if (gameov==1||grade==MAX_ASTEROIDS*20) {
        if (grade==MAX_ASTEROIDS*20) {
            c="You win!!! press[o] restarted,Grade- 0 ";
            
        }else{
            c="Game Over! press[o] restarted,Grade- 0 ";}
        glRasterPos2f(-0.5, 0);
        
    }else
    {
        c="Grade: ";
        glRasterPos2f(-0.9, 0.9);
    }
    
    for (c; *c!='\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    if (grade!=0) {
        
        char *x=intToChar(grade);
        for (x; *x!='\0'; x++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *x);
            
        }
        
    }
    if (grade==MAX_ASTEROIDS*20) {
        gameov=1;
    }
    glFlush();
    
}
static void    keyPress(int key, int x, int y)
{
    
    if (key==GLUT_KEY_LEFT) {
        printf("left\n");
        left=1;
    }
    else if(key==GLUT_KEY_RIGHT)
    {
        printf("right\n");
        right=1;
    }else if(key==GLUT_KEY_UP)
    {
        up=1;
        printf("up\n");
    }else if(key==GLUT_KEY_DOWN)
    {
        down=1;
        printf("Down\n");
    }
    glutPostRedisplay();
    
    
}
static void    keyRelease(int key, int x, int y)
{
    if (key==GLUT_KEY_LEFT) {
        
        left=0;
        printf("turn up\n");
    }
    else if(key==GLUT_KEY_RIGHT)
    {
        
        right=0;
        printf("turn up\n");
        
    }else if(key==GLUT_KEY_UP)
    {
        up=0;
        printf("turn up\n");
        
    }else if(key==GLUT_KEY_DOWN)
    {
        down=0;
        printf("turn up\n");
        
    }
    glutPostRedisplay();
    
}

void
myDisplay()
{
    /*
     *    display callback function
     */
    
    int    i;
    
    
    glClear(GL_COLOR_BUFFER_BIT);
    if (gameov==0) {
        
        glLoadIdentity();
        getgrade();
        glLoadIdentity();
        
        drawShip(&ship);
        
        
        
        for (i=0; i<MAX_PHOTONS; i++)
            if (photons[i].active)
                drawPhoton(&photons[i]);
        
        for (i=0; i<MAX_ASTEROIDS; i++)
        {
            
            drawAsteroid(&asteroids[i]);
            
        }
    }else
    {
        glLoadIdentity();
        getgrade();
    }
    
    glutSwapBuffers();
}

//***************************************- asteroids -*****************************************
static void    drawAsteroid(Asteroid *a)

{
    
    double    theta, r;
    int        i;
    glLoadIdentity();
    if (a->nVertices<6) {
        do{
            a->size=0.10;
            a->dx =myRandom(-0.9,0.9);
            a->dy = myRandom(-0.9, 0.9);
        }while ((a->dx>0.7||a->dx<-0.7)&&(a->dy>0.7||a->dy<-0.7));
        a->nVertices = 6+rand()%(MAX_VERTICES-6);
        a->dphi=myRandom(0, 360);
        glRotatef(a->phi, 0, 0, 1);
        myTranslate2D(a->x+a->dx, a->y+a->dy);
        glColor3f(0, 1, 1);
        
        glBegin(GL_LINE_LOOP);
        for (i=0; i<a->nVertices; i++)
        {
            theta = 2.0*M_PI*i/a->nVertices;
            r = a->size;
            a->coords[i].x = -r*sin(theta);
            a->coords[i].y = r*cos(theta);
            glVertex2f( a->coords[i].x,a->coords[i].y);
        }
        glEnd();
        a->active=1;
        glFlush();
    }
    else
    {   if(a->active==1)
    {{
        myTranslate2D(a->x+a->dx, a->y+a->dy);
        glRotatef(a->phi, 0, 0, 1);
        glColor3f(0, 1, 1);
        
        
        glBegin(GL_LINE_LOOP);
        for (i=0; i<a->nVertices; i++)
        {
            theta = 2.0*M_PI*i/a->nVertices;
            r = a->size;
            a->coords[i].x = -r*sin(theta);
            a->coords[i].y = r*cos(theta);
            glVertex2f(a->coords[i].x,a->coords[i].y);
        }
        glEnd();
        glFlush();
    }}
    }
    
}
//***************************************- Ships -*****************************************
static void    drawShip(Ship *s)
{
    s->x=0;
    s->y=0;
    myTranslate2D(s->x+s->dx,s->y+s->dy);
    glRotatef(s->phi, 0, 0, 1);
    glColor3f(ship.cor[0],ship.cor[1],ship.cor[2]);
    
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 0);
    glVertex2f(-0.02, -0.02);
    glVertex2f(0.02, -0.02);
    glEnd();
    glFlush();
    
}

//***************************************- photon -*****************************************

static void drawPhoton(Photon *p)
{
    glLoadIdentity();
    myTranslate2D(p->x+p->dx, p->y+p->dy);
    glColor3f(1,0,0);
    glBegin(GL_POINTS);
    glVertex2i(0, 0);
    glEnd();
    glFlush();
    
    
    
}

//***************************************- Keyboards  -*****************************************


static void    myKey(unsigned char key, int x, int y)
{
    //go left
    
    if(key==' ')
    {
        for (int i=0; i<MAX_PHOTONS; i++) {
            if (photons[i].active==0)
            {
                photons[i].active=1;
                photons[i].x=ship.dx;
                photons[i].y=ship.dy;
                photons[i].ang=ship.phi;
                
                //printf("%f %f\n",ship.dx,ship.dy);
                break;
            }
        }
    }
    else if(key=='p'||key=='P')
    {
        pau=1;
    }
    else if(key=='o'||key=='O')
    {
        gameov=0;
        
        if (grade==MAX_ASTEROIDS*20||dead==1) {
            
            
            for (int i=0; i<MAX_ASTEROIDS; i++) {
                asteroids[i].size=0.04;
                asteroids[i].active=1;
                photons[i].dx=0;
                photons[i].dy=0;
                photons[i].active=0;
            }
            grade=0;
            dead=0;
            
            
        }
        
        pau=0;
    }
    if (key=='I'||key=='i') {
        exit(0);
    }
    //turn around the ship
    if (key=='F'||key=='f') {
        if (turn==0) {
            turn=1;
        }else
        {
            turn=0;
        }
    }
    
    glutPostRedisplay();
    
}

//-----------------------------------moving ship method----------------------------------------

static void myIdle()
{
    if (gameov==1) {
        ship.dx=0;
        ship.dy=0;
        for (int t=0; t<MAX_ASTEROIDS; t++) {
            asteroids[t].nVertices=0;
            asteroids[t].active=1;
        }
        for (int i=0; i<MAX_PHOTONS; i++) {
            photons[i].dx=0;
            photons[i].dy=0;
            
            photons[i].active=0;
            
        }
        
        
    }
    else if (!pau) {
        
        /****************************************************************************************
         ---------------------------------- Touch event------------------------------------------
         ****************************************************************************************/
        for (int i=0; i<MAX_PHOTONS; i++) {
            for (int j=0; j<MAX_ASTEROIDS; j++) {
                double x1=asteroids[j].dx+asteroids[j].x,
                x2=photons[i].dx+photons[i].x,
                y1=asteroids[j].dy+asteroids[j].y,
                y2=photons[i].dy+photons[i].y;
                if (((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)<((asteroids[j].size)*(asteroids[j].size)))&&photons[i].active&&asteroids[j].active) {
                    grade+=10;
                    if (asteroids[j].size==0.1) {
                        asteroids[j].size=0.05;
                    }
                    else
                    {
                        asteroids[j].active=0;
                        photons[i].dx=0;
                        photons[i].dy=0;
                        photons[i].active=0;
                        
                    }
                    
                }
                
            }
        }
        for (int j=0; j<MAX_ASTEROIDS; j++) {
            if ((asteroids[j].dx-ship.dx)*(asteroids[j].dx-ship.dx)+(asteroids[j].dy-ship.dy)*(asteroids[j].dy-ship.dy)<(asteroids[j].size*asteroids[j].size))
            {
                printf("%f\n",ship.phi);
                gameov=1;
                dead=1;
                printf("11111\n");
            }
            else if((asteroids[j].dx-(ship.dx+transCoordX(-0.02, -0.02, ship.phi)))*(asteroids[j].dx-(ship.dx+transCoordX(-0.02, -0.02, ship.phi)))+(asteroids[j].dy-(ship.dy+transCoordY(-0.02, -0.02, ship.phi)))*(asteroids[j].dy-(ship.dy+transCoordY(-0.02, -0.02, ship.phi)))<(asteroids[j].size*asteroids[j].size))
            {
                printf("%f\n",ship.phi);
                
                dead=1;
                gameov=1;
                printf("22222\n");
                
                
                
                
            }
            else if ((asteroids[j].dx-(ship.dx+transCoordX(0.02, -0.02, ship.phi)))*(asteroids[j].dx-(ship.dx+transCoordX(0.02, -0.0, ship.phi)))+(asteroids[j].dy-(ship.dy+transCoordY(0.02, -0.02, ship.phi)))*(asteroids[j].dy-(ship.dy+transCoordY(0.02, -0.02, ship.phi)))<(asteroids[j].size*asteroids[j].size))
            {
                
                printf("%f\n",ship.phi);
                
                
                dead=1;
                gameov=1;
                printf("3333333\n");
                
            }
            
            /****************************************************************************************
             ---------------------------------- ships event------------------------------------------
             ****************************************************************************************/
            if (right==1) {
                if (ship.phi>0) {
                    ship.phi-=1;
                    
                }
                else
                {
                    ship.phi=360;
                }
            }
            if (left==1) {
                if (ship.phi<360) {
                    ship.phi+=1;
                    
                }
                else
                {
                    ship.phi=0;
                    
                }
                
            }
            if (up==1) {
                speedShip*=1.005;
                speedShip*=1.005;
            }
            if (down==1)
            {
                speedShip*=0.995;
                speedShip*=0.995;
            }
            
            if (ship.dx<=1&&ship.dx>=-1&&ship.dy<=1&&ship.dy>=-1) {
                if (ship.phi>=0&&ship.phi<90)
                {
                    ship.dy=ship.dy+speedShip*(cos(ship.phi*DEG2RAD));
                    ship.dx=ship.dx-speedShip*(sin(ship.phi*DEG2RAD));
                    
                }else if(ship.phi>=90&&ship.phi<180)
                {
                    ship.dy=ship.dy-speedShip*(sin((ship.phi-90)*DEG2RAD));
                    ship.dx=ship.dx-speedShip*(cos((ship.phi-90)*DEG2RAD));
                }else if (ship.phi>=180&&ship.phi<270)
                {
                    ship.dy=ship.dy-speedShip*(cos((ship.phi-180)*DEG2RAD));
                    ship.dx=ship.dx+speedShip*(sin((ship.phi-180)*DEG2RAD));
                }else
                {
                    ship.dy=ship.dy+speedShip*(sin((ship.phi-270)*DEG2RAD));
                    ship.dx=ship.dx+ShipSpeed*(cos((ship.phi-270)*DEG2RAD));
                    
                }
                
            }else if (ship.dx>1)
            {
                ship.dx=-1;
            }else if (ship.dx<-1)
            {
                ship.dx=1;
            }else if(ship.dy>1)
            {
                ship.dy=-1;
            }else if (ship.dy<-1)
            {
                ship.dy=1;
            }
            
            //--------------------------------------PHOTONS----------------------------------------------
            
            for (int i=0; i<MAX_PHOTONS; i++) {
                
                if (photons[i].active) {
                    if (photons[i].dx<1&&photons[i].dx>-1&&photons[i].dy<1&&photons[i].dy>-1) {
                        if (photons[i].ang>=0&&photons[i].ang<90)
                        {
                            photons[i].dy=photons[i].dy+PhotonSPEED*(cos(photons[i].ang*DEG2RAD));
                            photons[i].dx=photons[i].dx-PhotonSPEED*(sin(photons[i].ang*DEG2RAD));
                            
                        }else if(photons[i].ang>=90&&photons[i].ang<180)
                        {
                            photons[i].dy=photons[i].dy-PhotonSPEED*(sin((photons[i].ang-90)*DEG2RAD));
                            photons[i].dx=photons[i].dx-PhotonSPEED*(cos((photons[i].ang-90)*DEG2RAD));
                        }else if (photons[i].ang>=180&&photons[i].ang<270)
                        {
                            photons[i].dy=photons[i].dy-PhotonSPEED*(cos((photons[i].ang-180)*DEG2RAD));
                            photons[i].dx=photons[i].dx+PhotonSPEED*(sin((photons[i].ang-180)*DEG2RAD));
                        }else
                        {
                            photons[i].dy=photons[i].dy+PhotonSPEED*(sin((photons[i].ang-270)*DEG2RAD));
                            photons[i].dx=photons[i].dx+PhotonSPEED*(cos((photons[i].ang-270)*DEG2RAD));
                            
                        }
                        
                    }
                    else
                    {
                        photons[i].dx=0;
                        photons[i].dy=0;
                        
                        photons[i].active=0;
                        
                    }
                    
                }
            }
            
            //--------------------------------------Asteroids----------------------------------------------
            
            for(int i=0;i<MAX_ASTEROIDS;i++)
            {
                if (asteroids[i].active) {
                    if (asteroids[i].phi<=360) {
                        asteroids[i].phi+=0.25;
                    }else
                    {
                        asteroids[i].phi=0;
                    }
                    if (asteroids[i].dx<0.95&&asteroids[i].dx>-0.95&&asteroids[i].dy<0.95&&asteroids[i].dy>-0.95)
                    {
                        asteroids[i].dy=asteroids[i].dy+AsteroidSpeed*(cos(asteroids[i].dphi*DEG2RAD));
                        asteroids[i].dx=asteroids[i].dx-AsteroidSpeed*(sin(asteroids[i].dphi*DEG2RAD));
                    }
                    else
                    {
                        asteroids[i].dphi=myRandom(0, 360);
                        asteroids[i].dy=asteroids[i].dy+AsteroidSpeed*(cos(asteroids[i].dphi*DEG2RAD));
                        asteroids[i].dx=asteroids[i].dx-AsteroidSpeed*(sin(asteroids[i].dphi*DEG2RAD));
                        
                        
                    }
                    
                }
                for (int j=0; j<MAX_ASTEROIDS; j++) {
                    if (i!=j&&(asteroids[i].dx-asteroids[j].dx)*(asteroids[i].dx-asteroids[j].dx)+(asteroids[i].dy-asteroids[j].dy)*(asteroids[i].dy-asteroids[j].dy)<(asteroids[i].size+asteroids[j].size)*(asteroids[i].size+asteroids[j].size))
                    {
                        asteroids[i].dphi=myRandom(0, 360);
                        asteroids[i].dy=asteroids[i].dy+AsteroidSpeed*10*(cos(asteroids[i].dphi*DEG2RAD));
                        asteroids[i].dx=asteroids[i].dx-AsteroidSpeed*10*(sin(asteroids[i].dphi*DEG2RAD));
                    }
                }
                // initAsteroid(&asteroids[i], 0.4, 0.4, 0.3);
            }
            
        }
        
    }
    glutPostRedisplay();
    
    
}
//*****************************************\\init//*******************************************
static void    init(void)
{
    glPointSize(3.0);
}
//**************************************************************************************
void
myReshape(int w, int h)
{
    /*
     
     */
    
    xMax = w;
    yMax = h;
    coord.x=1.0*w/2;
    coord.y=1.0*h/2;
    
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
}

//------------------------------------ Mouse control ---------------------------------------------------
static void myMouse(int button, int state, int x,int y)
{
    //control ship direction
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
        ship.phi=-(atan2((x-coord.x)-ship.dx*coord.x,(coord.y-y-ship.dy)-ship.dy*coord.y)+atan2(10, 0))*RAD2DEG+90;
        if (ship.phi<0) {
            ship.phi=ship.phi+360;
        }else if(ship.phi>360)
        {
            ship.phi=ship.phi-360;
            ship.phi=ship.phi;
        }
    }
}

//math method====================================================================================
double
myRandom(double min, double max)
{
    double    d;
    
    d = min+(max-min)*(rand()%0x7fff)/32767.0;
    
    return d;
}


static char* intToChar(int n)
{
    int x=log10(n)+1;
    int i=0;
    char *array=calloc(x, sizeof(char));
    for (i=x-1; i>=0; i--,n=n/10) {
        array[i]='0'+n%10;
    }
    return array;
}
static double transCoordX(double x,double y,double phi)
{
    
    return x*cos(phi*DEG2RAD)-y*sin(phi*DEG2RAD);
}
static double transCoordY(double x,double y,double phi)
{
    return y*cos(phi*DEG2RAD)+x*sin(phi*DEG2RAD);
}
























