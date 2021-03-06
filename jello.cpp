/*
  USC/Viterbi/Computer Science
  CSCI 520 - Spring 2018
  HW1 - Jello Cube

  Name:   Stuti Rastogi
  ID:     8006687469
  Email:  srrastog@usc.edu
  Date:   04/10/18
*/

#include <iostream>
#include "jello.h"
#include "showCube.h"
#include "input.h"
#include "physics.h"

using namespace std;

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
int shear=0, bend=0, structural=1, pause=0, viewingMode=1, saveScreenToFile=0;

struct world jello;

int windowWidth, windowHeight;

void myinit()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0,1.0,0.01,1000.0);

  // set background color to grey
  glClearColor(0.1, 0.1, 0.1, 0.0);

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

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

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // camera parameters are Phi, Theta, R
  gluLookAt(R * cos(Phi) * cos (Theta), R * sin(Phi) * cos (Theta), 
            R * sin (Theta), 0.0,0.0,0.0, 0.0,0.0,1.0);


  // changed lighting to make it look different
  // global ambient light
  GLfloat aGa[] = { 0.863, 0.078, 0.235, 0.1 };
  
  GLfloat lKa0[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat lKd0[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat lKs0[] = { 1.000, 0.937, 0.8350, 0.2 };

  GLfloat lKa1[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat lKd1[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat lKs1[] = { 1.000, 0.937, 0.8350, 0.2 };

  GLfloat lKa2[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat lKd2[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat lKs2[] = { 1.000, 0.937, 0.8350, 0.2 };

  GLfloat lKa3[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat lKd3[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat lKs3[] = { 1.000, 0.937, 0.8350, 0.2 };

  GLfloat lKa4[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat lKd4[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat lKs4[] = { 1.000, 0.937, 0.8350, 0.2 };

  GLfloat lKa5[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat lKd5[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat lKs5[] = { 1.000, 0.937, 0.8350, 0.2 };

  GLfloat lKa6[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat lKd6[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat lKs6[] = { 1.000, 0.937, 0.8350, 0.2 };

  GLfloat lKa7[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat lKd7[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat lKs7[] = { 1.000, 0.937, 0.8350, 0.2 };

  // light positions and directions
  GLfloat lP0[] = { 2, 2, 2, 1.0 };
  GLfloat lP1[] = { -2, 2, 2, 1.0 };
  GLfloat lP2[] = { 2, -2, 2, 1.0 };
  GLfloat lP3[] = { 2, 2, -2, 1.0 };
  GLfloat lP4[] = { 2, -2, -2, 1.0 };
  GLfloat lP5[] = { -2, 2, -2, 1.0 };
  GLfloat lP6[] = { -2, -2, 2, 1.0 };
  GLfloat lP7[] = { -2, -2, -2, 1.0 };
  
  // jelly material color
  GLfloat mKa[] = { 1.000, 0.894, 0.882, 0.1 };
  GLfloat mKd[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat mKs[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat mKe[] = { 0.0, 0.0, 0.0, 1.0 };

  // set up lighting
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, aGa);
  glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

  // set up cube color
  glMaterialfv(GL_FRONT, GL_AMBIENT, mKa);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mKd);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
  glMaterialfv(GL_FRONT, GL_EMISSION, mKe);
  glMaterialf(GL_FRONT, GL_SHININESS, 30);
    
  // macro to set up light i
  #define LIGHTSETUP(i)\
  glLightfv(GL_LIGHT##i, GL_POSITION, lP##i);\
  glLightfv(GL_LIGHT##i, GL_AMBIENT, lKa##i);\
  glLightfv(GL_LIGHT##i, GL_DIFFUSE, lKd##i);\
  glLightfv(GL_LIGHT##i, GL_SPECULAR, lKs##i);\
  glEnable(GL_LIGHT##i)
  
  LIGHTSETUP (0);
  LIGHTSETUP (1);
  LIGHTSETUP (2);
  LIGHTSETUP (3);
  LIGHTSETUP (4);
  LIGHTSETUP (5);
  LIGHTSETUP (6);
  LIGHTSETUP (7);

  // enable lighting
  glEnable(GL_LIGHTING);    
  glEnable(GL_DEPTH_TEST);

  // show the cube
  showCube(&jello);

  glDisable(GL_LIGHTING);

  // show the bounding box
  showBoundingBox();
 
  glutSwapBuffers();
}

void doIdle()
{
  char s[20]="picxxxx.ppm";
  int i;
  
  // save screen to file
  s[3] = 48 + (sprite / 1000);
  s[4] = 48 + (sprite % 1000) / 100;
  s[5] = 48 + (sprite % 100 ) / 10;
  s[6] = 48 + sprite % 10;

  if (saveScreenToFile==1)
  {
    saveScreenshot(windowWidth, windowHeight, s);
    saveScreenToFile=0; // save only once
    sprite++;
  }

  if (sprite >= 300) // allow only 300 snapshots
  {
    exit(0);	
  }

  if (pause == 0)
  {
    // one step of simulation
    if (strcmp(jello.integrator, "RK4") == 0)
    {
      RK4(&jello);
    }
    else if (strcmp(jello.integrator, "Euler") == 0)
    {
      Euler(&jello);
    }
  }
  glutPostRedisplay();
}

int main (int argc, char ** argv)
{
  if (argc<2)
  {  
    printf ("Oops! You didn't say the jello world file!\n");
    printf ("Usage: %s [worldfile]\n", argv[0]);
    exit(0);
  }

  readWorld(argv[1],&jello);

  glutInit(&argc,argv);
  
  /* double buffered window, use depth testing, 640x480 */
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  
  windowWidth = 640;
  windowHeight = 480;
  glutInitWindowSize (windowWidth, windowHeight);
  glutInitWindowPosition (0,0);
  glutCreateWindow ("Jello cube");

  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);

  /* replace with any animate code */
  glutIdleFunc(doIdle);

  /* callback for mouse drags */
  glutMotionFunc(mouseMotionDrag);

  /* callback for window size changes */
  glutReshapeFunc(reshape);

  /* callback for mouse movement */
  glutPassiveMotionFunc(mouseMotion);

  /* callback for mouse button changes */
  glutMouseFunc(mouseButton);

  /* register for keyboard events */
  glutKeyboardFunc(keyboardFunc);

  /* do initialization */
  myinit();

  /* forever sink in the black hole */
  glutMainLoop();

  return(0);
}

