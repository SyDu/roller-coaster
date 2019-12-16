//
//  main.c
//  Roller_Coaster
//
//  Created by mac on 2018-07-25.
//  Copyright © 2018 mac. All rights reserved.
//
/*
 B00750733
 */



/*
 keyboard
 press [spacebar]--> switch camera (inside<->outside)
 press [q]--> quit the project
 */
#include <stdio.h>
#include <GLUT/GLUT.h>
#include <math.h>
#define RAD2DEG 180.0/M_PI
#define DEG2RAD M_PI/180.0

#define myTranslate2D(x,y) glTranslated(x, y, 0.0)
#define myScale2D(x,y) glScalef(x, y, 1.0)
#define myRotate2D(angle) glRotatef(RAD2DEG*angle, 0.0, 0.0, 1.0)

//Attributes
//for each points data structure
typedef struct Points {
    float x;
    float y;
    float z;
}point;

float maxX,maxY,maxZ,theta,pointsize=18;
int camera=0;
float insidemove=0;
point pois[18],q,qp,qpp,n,w,v;
point eye,up,aim;//for glulookat


//  camera 0 outside camera state, camera 1 inside camera

// Announce method
void myTimer(int value);
void myDisplay(void);
void myReshape(int w,int h);
void drawGound(void);
void drawSky(void);
void curvedegrer1(float x);
void curvedegrer2(float x);
void curvedegrer3(float x);
void initPoints(void);
void getNWV(void);
void drawCurve(void);
void cover(void);
static void key(unsigned char key, int x, int y);

point setpoint(float x,float y,float z);

int main(int argc, const char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(650, 500);
    glutCreateWindow("RollerCoaster");
    //write the display
    glutDisplayFunc(myDisplay);
    //set the key
    glutKeyboardFunc(key);
    //reshape
    glutReshapeFunc(myReshape);
    //set timmer
    glutTimerFunc(33, myTimer, 0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    //fill the points
    initPoints();
    glutMainLoop();

    return 0;
}

void myTimer(int value)
{
    //control the insidemove smaller than size(make it move as circle)
    if (insidemove>=pointsize-3)
    {
        insidemove=0.0;

    }
    curvedegrer1(insidemove);
    //get q
    curvedegrer2(insidemove);
    //get qp
    eye.x=q.x;
    eye.y=q.y*1.25;
    eye.z=q.z;
    aim.x=eye.x+qp.x;
    aim.y=(q.y+qp.y)*1.25;
    aim.z=eye.z+qp.z;
    insidemove+=0.03;//inside move speed
    
    theta += 0.02;//outside move spped
    //printf("%f %f %f \n",q.x,q.y,q.z);
    glutPostRedisplay();
    glutTimerFunc(33, myTimer, value);
}

void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //choose Camera
    if(camera%2==0) //outside
    {
        gluLookAt( 100*cos(theta),  20,  -100*sin(theta),
                  0,  0,  0,
                  0,  1,  0);
    }
    else//inside
    {
        gluLookAt( eye.x,  eye.y,  eye.z,
                  aim.x,  aim.y,  aim.z,
                  up.x,  up.y,  up.z);
    }
    //set ground
    drawGound();
    //set sky
    glPushMatrix();
    glTranslated(0, -1.5, 0);
    glRotated(-90.0, 1, 0, 0);
    drawSky();
    glPopMatrix();
    cover();
    //set tracke
    drawCurve();
    glutSwapBuffers();

    
}
void cover()
{
    glBegin(GL_QUADS);
    glVertex3f(100, 100, -100);
    glVertex3f(100, 100, 100);
    glVertex3f(-100, 100, 100);
    glVertex3f(-100, 100, -100);
    glEnd();
}

void myReshape(int w,int h)
{
    maxX=100.0f*w/h;
    maxY=100.0f;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,w/h,0.5,1000);
    glMatrixMode(GL_MODELVIEW);
}
//set ground
void drawGound(void)
{
    glColor3f(0.2, 0.4, 0.4);
    glBegin(GL_QUADS);
    glVertex3f(100, 0, -100);
    glVertex3f(100, 0, 100);
    glVertex3f(-100, 0, 100);
    glVertex3f(-100, 0, -100);
    glEnd();
}
//set sky
void drawSky(void)
{
    GLUquadricObj *sky = gluNewQuadric();
    glColor3f(0.7, 0.7, 0.9);
    gluCylinder(sky, 100, 100, 200, 200, 200);
}
//set points
void initPoints(void)
{
    pois[0]=setpoint(-16, 30, 0);
    pois[1]=setpoint(-11, 16, -3);
    pois[2]=setpoint(-8, 25, -7);
    pois[3]=setpoint(-3, 26, -8);
    pois[4]=setpoint(2, 20, -7);
     pois[6]=setpoint(13, 30, -4);
    pois[7]=setpoint(16, 25, 1);
    pois[8]=setpoint(14, 20, 4);
    pois[9]=setpoint(11, 22, 7);
    pois[10]=setpoint(7, 12, 6);
    pois[11]=setpoint(-1, 28, 6);
    pois[12]=setpoint(-8, 20, 6);
    pois[13]=setpoint(-12, 25, 5);
    pois[14]=setpoint(-14, 21, 2);
    pois[15]=setpoint(-16, 30, 0);
    pois[16]=setpoint(-11, 16, -3);
    pois[17]=setpoint(-8, 25, -7);
    up.x=0,up.y=1,up.z=0;
}
//assistant method
point setpoint(float x,float y,float z)
{
    point p;
    p.x=x;
    p.y=y;
    p.z=z;
    return p;
}
//get q(u)
void curvedegrer1(float x)
{
    float t, r0, r1, r2, r3;
    int ind = floor(x);
    t = x - ind;
    r3 = (1.0/6.0) * ((1.0 - t) * (1.0 - t) * (1.0 - t));
    r2 = (1.0/6.0) * ((3.0 * t * t * t) - (6.0 * t * t) + 4.0);
    r1 = (1.0/6.0) * ((-3.0 * t * t * t) + (3.0 * t * t) + (3.0 * t) + 1.0);
    r0 = (1.0/6.0) * (t * t * t);
    q.x = (r3 * pois[ind].x) + (r2 * pois[ind+1].x) + (r1 * pois[ind+2].x) + (r0 * pois[ind+3].x);
    q.y = (r3 * pois[ind].y) + (r2 * pois[ind+1].y) + (r1 * pois[ind+2].y) + (r0 * pois[ind+3].y);
    q.z = (r3 * pois[ind].z) + (r2 * pois[ind+1].z) + (r1 * pois[ind+2].z) + (r0 * pois[ind+3].z);
    
}
//get q'(u)
void curvedegrer2(float x)
{
    float t, r0, r1, r2, r3;
    int ind = floor(x);
    t = x - ind;
    r3 = (-0.5) * ((1.0 - t) * (1.0 - t));
    r2 = (1.5) * (t * t) - (2.0 * t);
    r1 = (-1.5) * (t * t) + t + 0.5;
    r0 = (0.5) * (t * t);
    qp.x = (r3 * pois[ind].x) + (r2 * pois[ind+1].x) + (r1 * pois[ind+2].x) + (r0 * pois[ind+3].x);
    qp.y = (r3 * pois[ind].y) + (r2 * pois[ind+1].y) + (r1 * pois[ind+2].y) + (r0 * pois[ind+3].y);
    qp.z = (r3 * pois[ind].z) + (r2 * pois[ind+1].z) + (r1 * pois[ind+2].z) + (r0 * pois[ind+3].z);
}
//get q''(u)
void curvedegrer3(float x)
    {
        float t, r0, r1, r2, r3;
        int ind = floor(x);
        t = x - ind;
        r3 = (1.0 - t);
        r2 = (3.0 * t) - 2.0;
        r1 = (-3.0 * t) + 1.0;
        r0 = t;
        qpp.x = (r3 * pois[ind].x) + (r2 * pois[ind+1].x) + (r1 * pois[ind+2].x) + (r0 * pois[ind+3].x);
        qpp.y = (r3 * pois[ind].y) + (r2 * pois[ind+1].y) + (r1 * pois[ind+2].y) + (r0 * pois[ind+3].y);
        qpp.z = (r3 * pois[ind].z) + (r2 * pois[ind+1].z) + (r1 * pois[ind+2].z) + (r0 * pois[ind+3].z);
    }

//get  n,w,v
void getNWV(void)
{
    float x, y;;
    n.x = -qp.x; n.y = -qp.y; n.z = -qp.z;
    x = sqrt((qp.x * qp.x) + (qp.y * qp.y) + (qp.z * qp.z));
    n.x /= x;
    n.y /= x;
    n.z /= x;
    
    w.x = (up.y * n.z) - (up.z * n.y);
    w.y = (up.z * n.x) - (up.x * n.z);
    w.z = (up.x * n.y) - (up.y * n.x);
    y = sqrt((w.x * w.x) + (w.y * w.y) + (w.z * w.z));
    w.x /= y;
    w.y /= y;
    w.z /= y;
    v.x = (n.y * w.z) - (n.z * w.y);
    v.y = (n.z * w.x) - (n.x * w.z);
    v.z = (n.x * w.y) - (n.y * w.x);
}
//shape the curve
void drawCurve()
{
    glBegin(GL_QUAD_STRIP);
    glColor3f(1, 0.5, 0.0);
    for(float x = 0.0; x<pointsize-3 ; x += 0.005)
    {
        curvedegrer1(x);
        curvedegrer2(x);
        getNWV();
        v.x /= 2.0; v.y /= 2.0; v.z /= 2.0;
        w.x /= 2.0; w.y /= 2.0; w.z /= 2.0;
        q.x += 1.5*v.x - 1.5*w.x;
        q.y += 1.5*v.y - 1.5*w.y;
        q.z += 1.5*v.z - 1.5*w.z;
        glVertex3f(q.x + v.x + w.x, q.y + v.y + w.y, q.z + v.z + w.z);
        glVertex3f(q.x + v.x - w.x, q.y + v.y - w.y, q.z + v.z - w.z);
      //  printf("%f\n",q.x + v.x + w.x);
        
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(float x = 0.0; x<pointsize-3 ; x += 0.005)
    {
        curvedegrer1(x);
        curvedegrer2(x);
        getNWV();
        v.x /= 2.0; v.y /= 2.0; v.z /= 2.0;
        w.x /= 2.0; w.y /= 2.0; w.z /= 2.0;
        q.x += 1.5*v.x + 1.5*w.x;
        q.y += 1.5*v.y + 1.5*w.y;
        q.z += 1.5*v.z + 1.5*w.z;
        glVertex3f(q.x + v.x + w.x, q.y + v.y + w.y, q.z + v.z + w.z);
        glVertex3f(q.x + v.x - w.x, q.y + v.y - w.y, q.z + v.z - w.z);
        
    }
    glEnd();
    for(float i = 0; i < pointsize-3; i=i+2)
    {
        glBegin(GL_QUAD_STRIP);
        glColor3f(0, 0, 0);
        curvedegrer1(i);
        curvedegrer2(i);
        getNWV();
        n.x /= 5; n.y /= 5; n.z /= 5;
        w.x /= 5; w.y /= 5; w.z /= 5;
        glVertex3f(q.x - n.x + w.x, q.y - n.y + w.y, q.z - n.z + w.z);
        glVertex3f(q.x - n.x + w.x, -6.0 - n.y + w.y, q.z - n.z + w.z);
        glVertex3f(q.x + n.x + w.x, q.y + n.y + w.y, q.z + n.z + w.z);
        glVertex3f(q.x + n.x + w.x, -6.0 + n.y + w.y, q.z + n.z + w.z);
        glVertex3f(q.x + n.x - w.x, q.y + n.y - w.y, q.z + n.z - w.z);
        glVertex3f(q.x + n.x - w.x, -6.0 + n.y - w.y, q.z + n.z - w.z);
        glEnd();
    }
    glFlush();

}
//key control
static void key(unsigned char key, int x, int y)
{
    if (key==' ')
    {
        camera++;
    }
    if (key=='q')
    {
        exit(0);
    }
}



















