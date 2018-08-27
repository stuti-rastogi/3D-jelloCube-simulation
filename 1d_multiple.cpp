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

struct point pos1 = {0, 0, -1};
struct point pos2 = {2, 0, -1};
struct point restPos1 = pos1;
struct point restPos2 = pos2; 
struct point v1 = {0, 0, 0};
struct point v2 = {0, 0, 0};
double force2 = 50;       // applied on last mass
double force1 = 0;
double m = 1;
double acc1, acc2;
double ext1, ext2;
double dt = 0.05;
double k = 5;
double kd = 0.1;
double restLength = 2;

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
    glVertex3f(-2,3,-1);
    glVertex3f(-2,-3,-1);
    glColor4f(1,0,0,1);
    glVertex3f(-2,0,-1);
    glVertex3f(pos1.x, pos1.y, pos1.z);
    glVertex3f(pos1.x, pos1.y, pos1.z);
    glVertex3f(pos2.x, pos2.y, pos2.z);
  glEnd();

  glPointSize(18);
  glBegin(GL_POINTS);
    glVertex3f(pos1.x, pos1.y, pos1.z);
    glVertex3f(pos2.x, pos2.y, pos2.z);
  glEnd();

  glutSwapBuffers();
}

void doIdle()
{
  acc2 = force2 / m;
  acc1 = force1 / m;

  v1.x = v1.x + acc1 * dt;
  v2.x = v2.x + acc2 * dt;

  pos1.x = pos1.x + v1.x * dt;
  pos2.x = pos2.x + v2.x * dt;

  ext1 = pos1.x - restPos1.x;    // 0 was initial position
  ext2 = pos2.x - restPos2.x;

  force2 = (-k * ext2) + (-kd * v2.x);
  force1 = ((-k * ext1) + (-k * v1.x)) - force2; 
  glutPostRedisplay();
}

int main (int argc, char ** argv)
{
  glutInit(&argc,argv);
  
  /* double buffered window, use depth testing, 640x480 */
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  
  windowWidth = 640;
  windowHeight = 480;
  glutInitWindowSize (windowWidth, windowHeight);
  glutInitWindowPosition (0,0);
  glutCreateWindow ("1D spring");

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

