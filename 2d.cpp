/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

  Your name:
  <write your name here>

*/

#if defined(WIN32) || defined(linux)
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#endif

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define pi 3.141592653589793238462643383279

struct point 
{
   double x;
   double y;
   double z;
};

// computes crossproduct of three vectors, which are given as points
// struct point vector1, vector2, dest
// result goes into dest
#define pCROSSPRODUCT(vector1,vector2,dest)\
  CROSSPRODUCT( (vector1).x, (vector1).y, (vector1).z,\
                (vector2).x, (vector2).y, (vector2).z,\
                (dest).x, (dest).y, (dest).z )

// computes crossproduct of three vectors, which are specified by floating-point coordinates
// double x1,y1,z1,x2,y2,z2,x,y,z
// result goes into x,y,z
#define CROSSPRODUCT(x1,y1,z1,x2,y2,z2,x,y,z)\
\
  x = (y1) * (z2) - (y2) * (z1);\
  y = (x2) * (z1) - (x1) * (z2);\
  z = (x1) * (y2) - (x2) * (y1)

#define pDOTPRODUCT(vector1, vector2, result)\
\
  result = vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z;

// normalizes vector dest
// struct point dest
// result returned in dest
// must declare a double variable called 'length' somewhere inside the scope of the NORMALIZE macrp
// macro will change that variable
#define pNORMALIZE(dest)\
\
  length = sqrt((dest).x * (dest).x + (dest).y * (dest).y + (dest).z * (dest).z);\
  (dest).x /= length;\
  (dest).y /= length;\
  (dest).z /= length;

// copies vector source to vector dest
// struct point source,dest
#define pCPY(source,dest)\
\
  (dest).x = (source).x;\
  (dest).y = (source).y;\
  (dest).z = (source).z;
  
// assigns values x,y,z to point vector dest
// struct point dest
// double x,y,z
#define pMAKE(x,y,z,dest)\
\
  (dest).(x) = (x);\
  (dest).(y) = (y);\
  (dest).(z) = (z);

// sums points src1 and src2 to dest
// struct point src1,src2,dest
#define pSUM(src1,src2,dest)\
\
  (dest).x = (src1).x + (src2).x;\
  (dest).y = (src1).y + (src2).y;\
  (dest).z = (src1).z + (src2).z;

// dest = src1 - src2
// struct point src1,src2,dest
#define pDIFFERENCE(src1,src2,dest)\
\
  (dest).x = (src1).x - (src2).x;\
  (dest).y = (src1).y - (src2).y;\
  (dest).z = (src1).z - (src2).z;

// mulitplies components of point src by scalar and returns the result in dest
// struct point src,dest
// double scalar
#define pMULTIPLY(src,scalar,dest)\
\
  (dest).x = (src).x * (scalar);\
  (dest).y = (src).y * (scalar);\
  (dest).z = (src).z * (scalar);

#define pLENGTH(src,len)\
\
  len = sqrt(src.x*src.x + src.y*src.y + src.z*src.z);

struct point pos1 = {-2, 2, -1};
struct point pos2 = {2, 2, -1};
struct point pos3 = {2, -2, -1};
struct point pos4 = {-2, -2, -1};

struct point restPos1 = pos1;
struct point restPos2 = pos2;
struct point restPos3 = pos3;
struct point restPos4 = pos4;

double r12 = 4;
double r23 = 4;
double r34 = 4;
double r41 = 4;
double r13 = 4 * sqrt(2);
double r24 = 4 * sqrt(2);

// velocity of each mass
struct point v1 = {0, 0, 0};
struct point v2 = {0, 0, 0};
struct point v3 = {0, 0, 0};
struct point v4 = {0, 0, 0};

// total force on each mass
struct point force1 = {50, 0, 0};
struct point force2 = {-50, 0, 0};
struct point force3 = {0, 0, 0};
struct point force4 = {0, 0, 0};

struct point f21, f41, f31;
struct point f12, f32, f42;
struct point f23, f43, f13;
struct point f14, f34, f24;

// acceleration of each mass
struct point acc1 = {0, 0, 0};
struct point acc2 = {0, 0, 0};
struct point acc3 = {0, 0, 0};
struct point acc4 = {0, 0, 0};

struct point ext1 = {0, 0, 0};
struct point ext2 = {0, 0, 0};
struct point ext3 = {0, 0, 0};
struct point ext4 = {0, 0, 0};

double m = 1;
double dt = 0.005;
double k = 10;
double kd = 0.2;

// camera parameters
double Theta = pi / 6;
double Phi = pi / 6;
double R = 6;

// mouse control
int g_iMenuId;
int g_vMousePos[2];
int g_iLeftMouseButton,g_iMiddleMouseButton,g_iRightMouseButton;

// number of images saved to disk so far
int sprite=0;

// these variables control what is displayed on screen
int shear=0, bend=0, structural=1, pause=0, viewingMode=0, saveScreenToFile=0;

int windowWidth, windowHeight;

void myinit()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0,1.0,0.01,1000.0);

  // set background color to grey
  glClearColor(0.8, 0.8, 0.8, 0.0);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_LINE_SMOOTH);

  return; 
}

void reshape(int w, int h) 
{
  // Prevent a divide by zero, when h is zero.
  // You can't make a window of zero height.
  if(h == 0)
    h = 1;

  glViewport(0, 0, w, h);
  cout << "w: " << w << " and h: " << h << endl;
  // Reset the coordinate system before modifying
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Set the perspective
  double aspectRatio = 1.0 * w / h;
  gluPerspective(60.0f, aspectRatio, 0.01f, 1000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 

  windowWidth = w;
  windowHeight = h;

  glutPostRedisplay();
}

void keyboardFunc (unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27:
      exit(0);
      break;
  }
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // camera parameters are Phi, Theta, R
  gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

  glEnable(GL_DEPTH_TEST);

  glBegin(GL_LINES);
    glColor4f(0,0,0,1);
    glVertex3f(pos1.x, pos1.y, pos1.z);
    glVertex3f(pos2.x, pos2.y, pos2.z);

    glVertex3f(pos2.x, pos2.y, pos2.z);
    glVertex3f(pos3.x, pos3.y, pos3.z);

    glVertex3f(pos3.x, pos3.y, pos3.z);
    glVertex3f(pos4.x, pos4.y, pos4.z);

    glVertex3f(pos4.x, pos4.y, pos4.z);
    glVertex3f(pos1.x, pos1.y, pos1.z);

    glVertex3f(pos4.x, pos4.y, pos4.z);
    glVertex3f(pos2.x, pos2.y, pos2.z);

    glVertex3f(pos1.x, pos1.y, pos1.z);
    glVertex3f(pos3.x, pos3.y, pos3.z);
  glEnd();

  glPointSize(18);
  glBegin(GL_POINTS);
    glVertex3f(pos1.x, pos1.y, pos1.z);
    glVertex3f(pos2.x, pos2.y, pos2.z);
    glVertex3f(pos3.x, pos3.y, pos3.z);
    glVertex3f(pos4.x, pos4.y, pos4.z);
  glEnd();

  glutSwapBuffers();
}

void doIdle()
{
  // calculate acceleration
  pMULTIPLY(force1, 1.0/m, acc1);
  pMULTIPLY(force2, 1.0/m, acc2);
  pMULTIPLY(force3, 1.0/m, acc3);
  pMULTIPLY(force4, 1.0/m, acc4);

  struct point temp;

  // update velocity
  pMULTIPLY(acc1, dt, temp);
  pSUM(v1, temp, v1);
  pMULTIPLY(acc2, dt, temp);
  pSUM(v2, temp, v2);
  pMULTIPLY(acc3, dt, temp);
  pSUM(v3, temp, v3);
  pMULTIPLY(acc4, dt, temp);
  pSUM(v4, temp, v4);

  // update position
  pMULTIPLY(v1, dt, temp);
  pSUM(pos1, temp, pos1);
  pMULTIPLY(v2, dt, temp);
  pSUM(pos2, temp, pos2);
  pMULTIPLY(v3, dt, temp);
  pSUM(pos3, temp, pos3);
  pMULTIPLY(v4, dt, temp);
  pSUM(pos4, temp, pos4);

  // calculate all forces
  struct point L, damping;
  double L_len, scaleForF, dot;
  
  // F12, F21
  pDIFFERENCE(pos2, pos1, L);
  pLENGTH(L, L_len);
  scaleForF = -k * (L_len - r12) / L_len; 
  pMULTIPLY(L, scaleForF, f12);
  pMULTIPLY(f12, -1.0, f21);

  // damping
  pDIFFERENCE(v2, v1, temp);
  pDOTPRODUCT(temp, L, dot);
  cout << "Dot: " << dot << endl;
  scaleForF = -kd * (dot / L_len) / L_len;
  cout << "scaleForF: " << scaleForF << endl << endl;
  pMULTIPLY(L, scaleForF, temp);
  pSUM(temp, f12, f12);
  pMULTIPLY(temp, -1.0, temp);
  pSUM(temp, f21, f21);

  // F32, F23
  pDIFFERENCE(pos2, pos3, L);
  pLENGTH(L, L_len);
  scaleForF = -k * (L_len - r23) / L_len; 
  pMULTIPLY(L, scaleForF, f32);
  pMULTIPLY(f32, -1.0, f23);

  // damping
  pDIFFERENCE(v2, v3, temp);
  pDOTPRODUCT(temp, L, dot);
  scaleForF = -kd * (dot / L_len) / L_len;
  pMULTIPLY(L, scaleForF, temp);
  pSUM(temp, f32, f32);
  pMULTIPLY(temp, -1.0, temp);
  pSUM(temp, f23, f23);

  // F34, F43
  pDIFFERENCE(pos4, pos3, L);
  pLENGTH(L, L_len);
  scaleForF = -k * (L_len - r34) / L_len; 
  pMULTIPLY(L, scaleForF, f34);
  pMULTIPLY(f34, -1.0, f43);

  // damping
  pDIFFERENCE(v4, v3, temp);
  pDOTPRODUCT(temp, L, dot);
  scaleForF = -kd * (dot / L_len) / L_len;
  pMULTIPLY(L, scaleForF, temp);
  pSUM(temp, f34, f34);
  pMULTIPLY(temp, -1.0, temp);
  pSUM(temp, f43, f43);

  // F41, F14
  pDIFFERENCE(pos1, pos4, L);
  pLENGTH(L, L_len);
  scaleForF = -k * (L_len - r41) / L_len; 
  pMULTIPLY(L, scaleForF, f41);
  pMULTIPLY(f41, -1.0, f14);

  // damping
  pDIFFERENCE(v1, v4, temp);
  pDOTPRODUCT(temp, L, dot);
  scaleForF = -kd * (dot / L_len) / L_len;
  pMULTIPLY(L, scaleForF, temp);
  pSUM(temp, f41, f41);
  pMULTIPLY(temp, -1.0, temp);
  pSUM(temp, f14, f14);

  // // F42, F24
  // pDIFFERENCE(pos2, pos4, L);
  // pLENGTH(L, L_len);
  // scaleForF = -k * (L_len - r24) / L_len; 
  // pMULTIPLY(L, scaleForF, f42);
  // pMULTIPLY(f42, -1.0, f24);

  // // damping
  // pDIFFERENCE(v2, v4, temp);
  // pDOTPRODUCT(temp, L, dot);
  // scaleForF = -kd * (dot / L_len) / L_len;
  // pMULTIPLY(L, scaleForF, temp);
  // pSUM(temp, f42, f42);
  // pMULTIPLY(temp, -1.0, temp);
  // pSUM(temp, f24, f24);

  // // F13, F31
  // pDIFFERENCE(pos3, pos1, L);
  // pLENGTH(L, L_len);
  // scaleForF = -k * (L_len - r13) / L_len; 
  // pMULTIPLY(L, scaleForF, f13);
  // pMULTIPLY(f13, -1.0, f31);

  // // damping
  // pDIFFERENCE(v3, v1, temp);
  // pDOTPRODUCT(temp, L, dot);
  // scaleForF = -kd * (dot / L_len) / L_len;
  // pMULTIPLY(L, scaleForF, temp);
  // pSUM(temp, f13, f13);
  // pMULTIPLY(temp, -1.0, temp);
  // pSUM(temp, f31, f31);



  pSUM(f21, f41, temp);
  pSUM(temp, f31, force1);

  pSUM(f12, f32, temp);
  pSUM(temp, f42, force2);

  pSUM(f23, f43, temp);
  pSUM(temp, f13, force3);

  pSUM(f34, f14, temp);
  pSUM(temp, f24, force4);

  glutPostRedisplay();
}

int main (int argc, char ** argv)
{
  glutInit(&argc,argv);
  
  /* double buffered window, use depth testing, 640x480 */
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  
  windowWidth = 1024;
  windowHeight = 1024;
  glutInitWindowSize (windowWidth, windowHeight);
  glutInitWindowPosition (0,0);
  glutCreateWindow ("2D spring network");

  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);

  /* replace with any animate code */
  glutIdleFunc(doIdle);

  /* register for keyboard events */
  glutKeyboardFunc(keyboardFunc);

  /* do initialization */
  myinit();

  /* forever sink in the black hole */
  glutMainLoop();

  return(0);
}

