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
#define pMAKE(x1,y1,z1,dest)\
\
  (dest).x = (x1);\
  (dest).y = (y1);\
  (dest).z = (z1);

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

struct point pos[2][2][2];
struct point vel [2][2][2];
struct point acc [2][2][2];
struct point force [2][2][2];

double r_structural = 4;
double r_shear = 4 * sqrt(2);

double m = 1;
double dt = 0.04;
double k_hook = 20;
double kd = 0.1;

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
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k =0; k < 2; k++)
      {
        pMAKE(0, 0, 0, vel[i][j][k]);
        pMAKE(0, 0, 0, acc[i][j][k]);
        pMAKE(0, 0, 0, force[i][j][k]);
      }
    }
  }

  pMAKE(2, 2, 2, pos[0][0][0]);
  pMAKE(2, 2, -2, pos[0][0][1]);
  pMAKE(2, -2, 2, pos[0][1][0]);
  pMAKE(2, -2, -2, pos[0][1][1]);
  pMAKE(-2, 2, 2, pos[1][0][0]);
  pMAKE(-2, 2, -2, pos[1][0][1]);
  pMAKE(-2, -2, 2, pos[1][1][0]);
  pMAKE(-2, -2, -2, pos[1][1][1]);

  pMAKE(-110, 0, 0, force[0][0][0]);
  pMAKE(110, 0, 0, force[1][0][0]);
  pMAKE(-110, 0, 0, force[0][1][0]);
  pMAKE(110, 0, 0, force[1][1][0]);

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
  gluLookAt(5.0, 6.0, 7.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glColor4f(0,0,0,1);
  glEnable(GL_DEPTH_TEST);

  glPointSize(18);
  glBegin(GL_POINTS);
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        for (int k = 0; k < 2; k++)
        {
          glVertex3f(pos[i][j][k].x, pos[i][j][k].y, pos[i][j][k].z);
        }
      }
    }
  glEnd();

  glBegin(GL_LINES);
    glVertex3f(pos[0][0][0].x, pos[0][0][0].y, pos[0][0][0].z);
    glVertex3f(pos[1][0][0].x, pos[1][0][0].y, pos[1][0][0].z);

    glVertex3f(pos[0][0][0].x, pos[0][0][0].y, pos[0][0][0].z);
    glVertex3f(pos[0][0][1].x, pos[0][0][1].y, pos[0][0][1].z);

    glVertex3f(pos[0][0][0].x, pos[0][0][0].y, pos[0][0][0].z);
    glVertex3f(pos[0][1][0].x, pos[0][1][0].y, pos[0][1][0].z);

    glVertex3f(pos[0][1][1].x, pos[0][1][1].y, pos[0][1][1].z);
    glVertex3f(pos[0][0][1].x, pos[0][0][1].y, pos[0][0][1].z);

    glVertex3f(pos[0][1][1].x, pos[0][1][1].y, pos[0][1][1].z);
    glVertex3f(pos[1][1][1].x, pos[1][1][1].y, pos[1][1][1].z);

    glVertex3f(pos[0][1][1].x, pos[0][1][1].y, pos[0][1][1].z);
    glVertex3f(pos[0][1][0].x, pos[0][1][0].y, pos[0][1][0].z);

    glVertex3f(pos[1][1][0].x, pos[1][1][0].y, pos[1][1][0].z);
    glVertex3f(pos[1][0][0].x, pos[1][0][0].y, pos[1][0][0].z);

    glVertex3f(pos[1][1][0].x, pos[1][1][0].y, pos[1][1][0].z);
    glVertex3f(pos[1][1][1].x, pos[1][1][1].y, pos[1][1][1].z);

    glVertex3f(pos[1][1][0].x, pos[1][1][0].y, pos[1][1][0].z);
    glVertex3f(pos[0][1][0].x, pos[0][1][0].y, pos[0][1][0].z);

    glVertex3f(pos[1][0][1].x, pos[1][0][1].y, pos[1][0][1].z);
    glVertex3f(pos[1][1][1].x, pos[1][1][1].y, pos[1][1][1].z);

    glVertex3f(pos[1][0][1].x, pos[1][0][1].y, pos[1][0][1].z);
    glVertex3f(pos[0][0][1].x, pos[0][0][1].y, pos[0][0][1].z);

    glVertex3f(pos[1][0][1].x, pos[1][0][1].y, pos[1][0][1].z);
    glVertex3f(pos[1][0][0].x, pos[1][0][0].y, pos[1][0][0].z);
  glEnd();
  glutSwapBuffers();
}

void doIdle()
{
  struct point temp;

  // update positions - using Euler
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 2; k++)
      {
        pMULTIPLY(force[i][j][k], 1.0/m, acc[i][j][k]);
        pMULTIPLY(acc[i][j][k], dt, temp);
        pSUM(vel[i][j][k], temp, vel[i][j][k]);
        pMULTIPLY(vel[i][j][k], dt, temp);
        pSUM(pos[i][j][k], temp, pos[i][j][k]);
      }
    }
  }

  // calculate new forces on each mass
  struct point L, neighbor;
  struct point temp1, temp2, temp3, temp2d;
  double L_len, scaleForF, dotResult;

  // structural
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 2; k++)
      {
        // 3 structural neighbors for each mass

        neighbor = pos[(i+1)%2][j][k];
        pDIFFERENCE(pos[i][j][k], neighbor, L);
        pLENGTH(L, L_len);
        scaleForF = -k_hook * ((L_len - r_structural) / L_len);
        pMULTIPLY(L, scaleForF, temp1);

        //damping
        neighbor = vel[(i+1)%2][j][k];
        pDIFFERENCE(vel[i][j][k], neighbor, temp);
        pDOTPRODUCT(temp, L, dotResult);
        scaleForF = (-kd * (dotResult / L_len)) / L_len;
        pMULTIPLY(L, scaleForF, temp);
        pSUM(temp1, temp, temp1);

        neighbor = pos[i][(j+1)%2][k];
        pDIFFERENCE(pos[i][j][k], neighbor, L);
        pLENGTH(L, L_len);
        scaleForF = -k_hook * (L_len - r_structural) / L_len;
        pMULTIPLY(L, scaleForF, temp2);

        //damping
        neighbor = vel[i][(j+1)%2][k];
        pDIFFERENCE(vel[i][j][k], neighbor, temp);
        pDOTPRODUCT(temp, L, dotResult);
        scaleForF = (-kd * (dotResult / L_len)) / L_len;
        pMULTIPLY(L, scaleForF, temp2d);
        pSUM(temp2, temp2d, temp2);

        neighbor = pos[i][j][(k+1)%2];
        pDIFFERENCE(pos[i][j][k], neighbor, L);
        pLENGTH(L, L_len);
        scaleForF = -k_hook * (L_len - r_structural) / L_len;
        pMULTIPLY(L, scaleForF, temp3);

        //damping
        neighbor = vel[i][j][(k+1)%2];
        pDIFFERENCE(vel[i][j][k], neighbor, temp);
        pDOTPRODUCT(temp, L, dotResult);
        scaleForF = (-kd * (dotResult / L_len)) / L_len;
        pMULTIPLY(L, scaleForF, temp);
        pSUM(temp3, temp, temp3);

        // add all structural forces
        pSUM(temp1, temp2, force[i][j][k]);
        pSUM(force[i][j][k], temp3, force[i][j][k]);
      }
    }
  }

  // for (int i = 0; i < 2; i++)
  // {
  //   for (int j = 0; j < 2; j++)
  //   {
  //     for (int k = 0; k < 2; k++)
  //     {

  //     }
  //   }
  // }
  // if (i == 0 && j == 0 && k == 0)
// {
//   cout << "L: (" << L.x << ", " << L.y << ", " << L.z << ")" << endl;
//   cout << "Mass velocity: (" << vel[i][j][k].x << ", " << vel[i][j][k].y << ", " << vel[i][j][k].z << ")" << endl;
//   cout << "Neighbor velocity: (" << neighbor.x << ", " << neighbor.y << ", " << neighbor.z << ")" << endl;
//   cout << "Difference: (" << temp.x << ", " << temp.y << ", " << temp.z << ")" << endl;
//   cout << "Dot: " << dotResult << endl;
//   cout << "Scale: " << scaleForF << endl;
//   cout << "Final force: (" << temp2d.x << ", " << temp2d.y << ", " << temp2d.z << ")" << endl << endl;
// }

  // shear

  //cout << "********* SHEAR ********** " << endl;
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 2; k++)
      {
        // 3 shear neighbors for each mass

        neighbor = pos[(i+1)%2][(j+1)%2][k];
        pDIFFERENCE(pos[i][j][k], neighbor, L);
        pLENGTH(L, L_len);
        scaleForF = -k_hook * (L_len - r_shear) / L_len;
        pMULTIPLY(L, scaleForF, temp);
        pSUM(force[i][j][k], temp, force[i][j][k]);

        //damping
        neighbor = vel[(i+1)%2][(j+1)%2][k];
        pDIFFERENCE(vel[i][j][k], neighbor, temp);
        pDOTPRODUCT(temp, L, dotResult);
        scaleForF = (-kd * (dotResult / L_len)) / L_len;
        pMULTIPLY(L, scaleForF, temp);
        pSUM(force[i][j][k], temp, force[i][j][k]);

        neighbor = pos[i][(j+1)%2][(k+1)%2];
        pDIFFERENCE(pos[i][j][k], neighbor, L);
        pLENGTH(L, L_len);
        scaleForF = -k_hook * (L_len - r_shear) / L_len;
        pMULTIPLY(L, scaleForF, temp);
        pSUM(force[i][j][k], temp, force[i][j][k]);

        //damping
        neighbor = vel[i][(j+1)%2][(k+1)%2];
        pDIFFERENCE(vel[i][j][k], neighbor, temp);
        pDOTPRODUCT(temp, L, dotResult);
        scaleForF = (-kd * (dotResult / L_len)) / L_len;
        pMULTIPLY(L, scaleForF, temp);
        pSUM(force[i][j][k], temp, force[i][j][k]);

        neighbor = pos[(i+1)%2][j][(k+1)%2];
        pDIFFERENCE(pos[i][j][k], neighbor, L);
        pLENGTH(L, L_len);
        scaleForF = -k_hook * (L_len - r_shear) / L_len;
        pMULTIPLY(L, scaleForF, temp);
        pSUM(force[i][j][k], temp, force[i][j][k]);

        //damping
        neighbor = vel[(i+1)%2][j][(k+1)%2];
        pDIFFERENCE(vel[i][j][k], neighbor, temp);
        pDOTPRODUCT(temp, L, dotResult);
        scaleForF = (-kd * (dotResult / L_len)) / L_len;
        pMULTIPLY(L, scaleForF, temp);
        pSUM(force[i][j][k], temp, force[i][j][k]);
      }
    }
  }
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

