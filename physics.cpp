/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

*/

#include "jello.h"
#include "physics.h"

// This function calculates the force at a point due to the external force
// field. We make use of trilinear interpolation
// Inputs:  jello - description of jello system
//          i, j, k - position of point in X, Y, Z
// Output:  The external force components in X, Y, Z
struct point calculateExternalForce (struct world * jello, int i, int j, int k)
{
  int fRes = jello->resolution;
  struct point interpolatedF;
  int x, y, z; 
  x = floor((fRes - 1)/4.0 * (jello->p[i][j][k].x + 2));
  y = floor((fRes - 1)/4.0 * (jello->p[i][j][k].y + 2));
  z = floor((fRes - 1)/4.0 * (jello->p[i][j][k].z + 2));

  // need to calculate differences for interpolation
  double x0 = -2 + (x * 4.0/(fRes - 1));
  double x1 = -2 + ((x+1) * 4.0/(fRes - 1));
  double y0 = -2 + (y * 4.0/(fRes - 1));
  double y1 = -2 + ((y+1) * 4.0/(fRes - 1));
  double z0 = -2 + (z * 4.0/(fRes - 1));
  double z1 = -2 + ((z+1) * 4.0/(fRes - 1));

  double xd = (jello->p[i][j][k].x - x0)/(x1 - x0);
  double yd = (jello->p[i][j][k].y - y0)/(y1 - y0);
  double zd = (jello->p[i][j][k].z - z0)/(z1 - z0);

  // fetch the force values at the 8 points of the grid
  struct point f000, f001, f010, f011, f100, f101, f110, f111;
  int p, q, r;
  p = x; q = y; r = z+1;
  pMAKE (jello->forceField[p * fRes * fRes + q * fRes + r].x, 
        jello->forceField[p * fRes * fRes + q * fRes + r].y, 
        jello->forceField[p * fRes * fRes + q * fRes + r].z, 
        f000);
  p = x; q = y+1; r = z+1;
  pMAKE (jello->forceField[p * fRes * fRes + q * fRes + r].x, 
        jello->forceField[p * fRes * fRes + q * fRes + r].y, 
        jello->forceField[p * fRes * fRes + q * fRes + r].z, 
        f001);
  p = x; q = y; r = z;
  pMAKE (jello->forceField[p * fRes * fRes + q * fRes + r].x, 
        jello->forceField[p * fRes * fRes + q * fRes + r].y, 
        jello->forceField[p * fRes * fRes + q * fRes + r].z, 
        f010);
  p = x; q = y+1; r = z;
  pMAKE (jello->forceField[p * fRes * fRes + q * fRes + r].x, 
        jello->forceField[p * fRes * fRes + q * fRes + r].y, 
        jello->forceField[p * fRes * fRes + q * fRes + r].z, 
        f011);
  p = x+1; q = y; r = z+1;
  pMAKE (jello->forceField[p * fRes * fRes + q * fRes + r].x, 
        jello->forceField[p * fRes * fRes + q * fRes + r].y, 
        jello->forceField[p * fRes * fRes + q * fRes + r].z, 
        f100);
  p = x+1; q = y+1; r = z+1;
  pMAKE (jello->forceField[p * fRes * fRes + q * fRes + r].x, 
        jello->forceField[p * fRes * fRes + q * fRes + r].y, 
        jello->forceField[p * fRes * fRes + q * fRes + r].z, 
        f101);
  p = x+1; q = y; r = z;
  pMAKE (jello->forceField[p * fRes * fRes + q * fRes + r].x, 
        jello->forceField[p * fRes * fRes + q * fRes + r].y, 
        jello->forceField[p * fRes * fRes + q * fRes + r].z, 
        f110);
  p = x+1; q = y+1; r = z;
  pMAKE (jello->forceField[p * fRes * fRes + q * fRes + r].x, 
        jello->forceField[p * fRes * fRes + q * fRes + r].y, 
        jello->forceField[p * fRes * fRes + q * fRes + r].z, 
        f111);

  //trilinear interpolation
  struct point temp1, temp2;
  struct point f00, f01, f10, f11;

  pMULTIPLY(f000, (1 - xd), temp1);
  pMULTIPLY(f100, xd, temp2);
  pSUM(temp1, temp2, f00);

  pMULTIPLY(f001, (1 - xd), temp1);
  pMULTIPLY(f101, xd, temp2);
  pSUM(temp1, temp2, f01);

  pMULTIPLY(f010, (1 - xd), temp1);
  pMULTIPLY(f110, xd, temp2);
  pSUM(temp1, temp2, f10);

  pMULTIPLY(f011, (1 - xd), temp1);
  pMULTIPLY(f111, xd, temp2);
  pSUM(temp1, temp2, f11);

  struct point f0, f1;
  pMULTIPLY(f00, (1 - yd), temp1);
  pMULTIPLY(f10, yd, temp2);
  pSUM(temp1, temp2, f0);

  pMULTIPLY(f00, (1 - zd), temp1);
  pMULTIPLY(f10, zd, temp2);
  pSUM(temp1, temp2, f0);

  pMULTIPLY(f01, (1 - zd), temp1);
  pMULTIPLY(f11, zd, temp2);
  pSUM(temp1, temp2, f1);

  pMULTIPLY(f0, (1 - yd), temp1);
  pMULTIPLY(f1, yd, temp2);
  pSUM(temp1, temp2, interpolatedF);
  return interpolatedF;
}

// This function calculates the force at a point due to the external force
// field. We make use of trilinear interpolation
// Inputs:  jello - description of jello system
// Output:  The acceleration for all points stored in array a
void computeAcceleration(struct world * jello, struct point a[8][8][8])
{
  struct point f[8][8][8];            // total force on each point 

  // rest lengths for springs
  double rest_structural = 1.0/7.0;
  double rest_bend = 2.0/7.0;
  double rest_shear = sqrt(2.0/49.0);
  double rest_diagonal = sqrt(3.0/49.0);
 
  // each type of force at each point
  struct point f_structural[8][8][8];
  struct point f_shear[8][8][8];
  struct point f_bend[8][8][8];
  struct point f_diagonal[8][8][8];
  struct point f_external[8][8][8];

  // initialise all forces to 0
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        pMAKE(0,0,0,f_structural[i][j][k]);
        pMAKE(0,0,0,f_shear[i][j][k]);
        pMAKE(0,0,0,f_bend[i][j][k]);
        pMAKE(0,0,0,f_diagonal[i][j][k]);
      }
    }
  }

  // get force due to external force field at each point
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        f_external[i][j][k] = calculateExternalForce (jello, i, j, k);
      }
    }
  }

  // intermediate variables
  struct point L, temp, neighbor;
  double L_len, scaleForF, dotResult;

  // structural springs
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        // neighbor 1
        if (i != 7)
        {
          neighbor = jello->p[i+1][j][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_structural) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);

          // damping
          neighbor = jello->v[i+1][j][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);
        }

        // neighbor 2
        if (i != 0)
        {
          neighbor = jello->

          p[i-1][j][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_structural) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);

          // damping
          neighbor = jello->v[i-1][j][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);
        }

        // neighbor 3
        if (j != 7)
        {
          neighbor = jello->p[i][j+1][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_structural) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);

          // damping
          neighbor = jello->v[i][j+1][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);
        }

        // neighbor 4
        if (j != 0)
        {
          neighbor = jello->p[i][j-1][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_structural) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);

          // damping
          neighbor = jello->v[i][j-1][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);
        }

        // neighbor 5
        if (k != 7)
        {
          neighbor = jello->p[i][j][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_structural) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);

          // damping
          neighbor = jello->v[i][j][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);
        }

        // neighbor 6
        if (k != 0)
        {
          neighbor = jello->p[i][j][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_structural) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);

          // damping
          neighbor = jello->v[i][j][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_structural[i][j][k], temp, f_structural[i][j][k]);
        }
      }
    }
  }

  // shear springs
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        // neighbor 1
        if (i != 7 && j != 7)
        {
          neighbor = jello->p[i+1][j+1][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          // damping
          neighbor = jello->v[i+1][j+1][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 2
        if (i != 0 && j != 7)
        {
          neighbor = jello->p[i-1][j+1][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i-1][j+1][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 3
        if (i != 7 && j != 0)
        {
          neighbor = jello->p[i+1][j-1][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i+1][j-1][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 4
        if (i != 0 && j != 0)
        {
          neighbor = jello->p[i-1][j-1][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i-1][j-1][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 5
        if (i != 7 && k != 7)
        {
          neighbor = jello->p[i+1][j][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i+1][j][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 6
        if (i != 0 && k != 7)
        {
          neighbor = jello->p[i-1][j][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i-1][j][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 7
        if (i != 7 && k != 0)
        {
          neighbor = jello->p[i+1][j][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i+1][j][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 8
        if (i != 0 && k != 0)
        {
          neighbor = jello->p[i-1][j][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i-1][j][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 9
        if (k != 7 && j != 7)
        {
          neighbor = jello->p[i][j+1][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i][j+1][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 10
        if (k != 0 && j != 7)
        {
          neighbor = jello->p[i][j+1][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i][j+1][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 11
        if (k != 7 && j != 0)
        {
          neighbor = jello->p[i][j-1][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i][j-1][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }

        // neighbor 12
        if (k != 0 && j != 0)
        {
          neighbor = jello->p[i][j-1][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_shear) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);

          neighbor = jello->v[i][j-1][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_shear[i][j][k], temp, f_shear[i][j][k]);
        }
      }
    }
  }

  // bend springs
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        // neighbor 1
        if (i+2 <= 7)
        {
          neighbor = jello->p[i+2][j][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_bend) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);

          neighbor = jello->v[i+2][j][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);
        }

        // neighbor 2
        if (i-2 >= 0)
        {
          neighbor = jello->p[i-2][j][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_bend) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);

          neighbor = jello->v[i-2][j][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);
        }

        // neighbor 3
        if (j+2 <= 7)
        {
          neighbor = jello->p[i][j+2][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_bend) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);

          neighbor = jello->v[i][j+2][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);
        }

        // neighbor 4
        if (j-2 >= 0)
        {
          neighbor = jello->p[i][j-2][k];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_bend) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);

          neighbor = jello->v[i][j-2][k];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);
        }

        // neighbor 5
        if (k+2 <= 7)
        {
          neighbor = jello->p[i][j][k+2];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_bend) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);

          neighbor = jello->v[i][j][k+2];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);
        }

        // neighbor 6
        if (k-2 >= 0)
        {
          neighbor = jello->p[i][j][k-2];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_bend) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);

          neighbor = jello->v[i][j][k-2];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_bend[i][j][k], temp, f_bend[i][j][k]);
        }
      }
    }
  }

  // diagonal springs - for more stability
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        // neighbor 1
        if (i+1 <= 7 && j+1 <= 7 && k+1 <= 7)
        {
          neighbor = jello->p[i+1][j+1][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_diagonal) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);

          neighbor = jello->v[i+1][j+1][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);
        }

        // neighbor 2
        if (i+1 <= 7 && j+1 <= 7 && k-1 >= 0)
        {
          neighbor = jello->p[i+1][j+1][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_diagonal) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);

          neighbor = jello->v[i+1][j+1][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);
        }

        // neighbor 3
        if (i-1 >= 0 && j+1 <= 7 && k+1 <= 7)
        {
          neighbor = jello->p[i-1][j+1][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_diagonal) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);

          neighbor = jello->v[i-1][j+1][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);
        }

        // neighbor 4
        if (i-1 >= 0 && j+1 <= 7 && k-1 >= 0)
        {
          neighbor = jello->p[i-1][j+1][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_diagonal) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);

          neighbor = jello->v[i-1][j+1][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);
        }

        // neighbor 5
        if (i+1 <= 7 && j-1 >= 0 && k+1 <= 7)
        {
          neighbor = jello->p[i+1][j-1][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_diagonal) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);

          neighbor = jello->v[i+1][j-1][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);
        }

        // neighbor 6
        if (i+1 <= 7 && j-1 >= 0 && k-1 >= 0)
        {
          neighbor = jello->p[i+1][j-1][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_diagonal) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);

          neighbor = jello->v[i+1][j-1][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);
        }

        // neighbor 7
        if (i-1 >= 0 && j-1 >= 0 && k+1 <= 7)
        {
          neighbor = jello->p[i-1][j-1][k+1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_diagonal) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);

          neighbor = jello->v[i-1][j-1][k+1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);
        }

        // neighbor 8
        if (i-1 >= 0 && j-1 >= 0 && k-1 >= 0)
        {
          neighbor = jello->p[i-1][j-1][k-1];
          pDIFFERENCE(jello->p[i][j][k], neighbor, L);
          pLENGTH(L, L_len);
          scaleForF = -jello->kElastic * ((L_len - rest_diagonal) / L_len);
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);

          neighbor = jello->v[i-1][j-1][k-1];
          pDIFFERENCE(jello->v[i][j][k], neighbor, temp);
          pDOTPRODUCT(temp, L, dotResult);
          scaleForF = (-jello->dElastic * (dotResult / L_len)) / L_len;
          pMULTIPLY(L, scaleForF, temp);
          pSUM(f_diagonal[i][j][k], temp, f_diagonal[i][j][k]);
        }
      }
    }
  }

  // add all forces together
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        pSUM(f_structural[i][j][k], f_shear[i][j][k], temp);
        pSUM(temp, f_bend[i][j][k], temp);
        pSUM(temp, f_diagonal[i][j][k], temp);
        pSUM(temp, f_external[i][j][k], f[i][j][k]);
      }
    }
  }

  // collision detection
  // since we apply a force after penetration, it acts on the cube after
  // penetration has taken place
  // to avoid that and make it look like the jello cube bounces on just
  // touching the surface we have an epsilon value
  double epsilon = 0.05;
  double ext;             // extension of collision spring
  struct point f_penalty, penalty_damping;

  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        if (jello->p[i][j][k].x >= 2-epsilon)
        {
          ext = jello->p[i][j][k].x - (2-epsilon);
          pMAKE(-jello->kCollision * ext, 0, 0, f_penalty);
          pMAKE(-jello->dCollision * jello->v[i][j][k].x, 0, 0, 
                penalty_damping);
          pSUM(f_penalty, penalty_damping, f_penalty);
          pSUM(f[i][j][k], f_penalty, f[i][j][k]);
        }
        if (jello->p[i][j][k].x <= -2+epsilon)
        {
          ext = (-2+epsilon) - jello->p[i][j][k].x;
          pMAKE(jello->kCollision * ext, 0, 0, f_penalty);
          pMAKE(jello->dCollision * jello->v[i][j][k].x, 0, 0, 
                penalty_damping);
          pSUM(f_penalty, penalty_damping, f_penalty);
          pSUM(f[i][j][k], f_penalty, f[i][j][k]);
        }

        if (jello->p[i][j][k].y >= 2-epsilon)
        {
          ext = jello->p[i][j][k].y - (2-epsilon);
          pMAKE(0, -jello->kCollision * ext, 0, f_penalty);
          pMAKE(0, -jello->dCollision * jello->v[i][j][k].y, 0, 
                penalty_damping);
          pSUM(f_penalty, penalty_damping, f_penalty);
          pSUM(f[i][j][k], f_penalty, f[i][j][k]);
        }
        if (jello->p[i][j][k].y <= -2+epsilon)
        {
          ext = (-2+epsilon) - jello->p[i][j][k].y;
          pMAKE(0, jello->kCollision * ext, 0, f_penalty);
          pMAKE(0, jello->dCollision * jello->v[i][j][k].y, 0, 
                penalty_damping);
          pSUM(f_penalty, penalty_damping, f_penalty);
          pSUM(f[i][j][k], f_penalty, f[i][j][k]);          
        }

        if (jello->p[i][j][k].z >= 2-epsilon)
        {
          ext = jello->p[i][j][k].z - (2-epsilon);
          pMAKE(0, 0, -jello->kCollision * ext, f_penalty);
          pMAKE(0, 0, -jello->dCollision * jello->v[i][j][k].z, 
                penalty_damping);
          pSUM(f_penalty, penalty_damping, f_penalty);
          pSUM(f[i][j][k], f_penalty, f[i][j][k]);
        }
        if (jello->p[i][j][k].z <= -2+epsilon)
        {
          ext = (-2+epsilon) - jello->p[i][j][k].z;
          pMAKE(0, 0, jello->kCollision * ext, f_penalty);
          pMAKE(0, 0, jello->dCollision * jello->v[i][j][k].z, 
                penalty_damping);
          pSUM(f_penalty, penalty_damping, f_penalty);
          pSUM(f[i][j][k], f_penalty, f[i][j][k]);          
        }
      }
    }
  }

  // calculate acceleration from force
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        pMULTIPLY(f[i][j][k], 1.0/jello->mass, a[i][j][k]);
      }
    }
  }
}

/* performs one step of Euler Integration */
/* as a result, updates the jello structure */
void Euler(struct world * jello)
{
  int i,j,k;
  point a[8][8][8];

  computeAcceleration(jello, a);
  
  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
        jello->p[i][j][k].x += jello->dt * jello->v[i][j][k].x;
        jello->p[i][j][k].y += jello->dt * jello->v[i][j][k].y;
        jello->p[i][j][k].z += jello->dt * jello->v[i][j][k].z;
        jello->v[i][j][k].x += jello->dt * a[i][j][k].x;
        jello->v[i][j][k].y += jello->dt * a[i][j][k].y;
        jello->v[i][j][k].z += jello->dt * a[i][j][k].z;

      }
}

/* performs one step of RK4 Integration */
/* as a result, updates the jello structure */
void RK4(struct world * jello)
{
  point F1p[8][8][8], F1v[8][8][8], 
        F2p[8][8][8], F2v[8][8][8],
        F3p[8][8][8], F3v[8][8][8],
        F4p[8][8][8], F4v[8][8][8];

  point a[8][8][8];


  struct world buffer;

  int i,j,k;

  buffer = *jello; // make a copy of jello

  computeAcceleration(jello, a);

  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
         pMULTIPLY(jello->v[i][j][k],jello->dt,F1p[i][j][k]);
         pMULTIPLY(a[i][j][k],jello->dt,F1v[i][j][k]);
         pMULTIPLY(F1p[i][j][k],0.5,buffer.p[i][j][k]);
         pMULTIPLY(F1v[i][j][k],0.5,buffer.v[i][j][k]);
         pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
         pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
      }

  computeAcceleration(&buffer, a);

  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
         // F2p = dt * buffer.v;
         pMULTIPLY(buffer.v[i][j][k],jello->dt,F2p[i][j][k]);
         // F2v = dt * a(buffer.p,buffer.v);     
         pMULTIPLY(a[i][j][k],jello->dt,F2v[i][j][k]);
         pMULTIPLY(F2p[i][j][k],0.5,buffer.p[i][j][k]);
         pMULTIPLY(F2v[i][j][k],0.5,buffer.v[i][j][k]);
         pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
         pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
      }

  computeAcceleration(&buffer, a);

  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
         // F3p = dt * buffer.v;
         pMULTIPLY(buffer.v[i][j][k],jello->dt,F3p[i][j][k]);
         // F3v = dt * a(buffer.p,buffer.v);     
         pMULTIPLY(a[i][j][k],jello->dt,F3v[i][j][k]);
         pMULTIPLY(F3p[i][j][k],0.5,buffer.p[i][j][k]);
         pMULTIPLY(F3v[i][j][k],0.5,buffer.v[i][j][k]);
         pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
         pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
      }
         
  computeAcceleration(&buffer, a);


  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
         // F3p = dt * buffer.v;
         pMULTIPLY(buffer.v[i][j][k],jello->dt,F4p[i][j][k]);
         // F3v = dt * a(buffer.p,buffer.v);     
         pMULTIPLY(a[i][j][k],jello->dt,F4v[i][j][k]);

         pMULTIPLY(F2p[i][j][k],2,buffer.p[i][j][k]);
         pMULTIPLY(F3p[i][j][k],2,buffer.v[i][j][k]);
         pSUM(buffer.p[i][j][k],buffer.v[i][j][k],buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],F1p[i][j][k],buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],F4p[i][j][k],buffer.p[i][j][k]);
         pMULTIPLY(buffer.p[i][j][k],1.0 / 6,buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],jello->p[i][j][k],jello->p[i][j][k]);

         pMULTIPLY(F2v[i][j][k],2,buffer.p[i][j][k]);
         pMULTIPLY(F3v[i][j][k],2,buffer.v[i][j][k]);
         pSUM(buffer.p[i][j][k],buffer.v[i][j][k],buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],F1v[i][j][k],buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],F4v[i][j][k],buffer.p[i][j][k]);
         pMULTIPLY(buffer.p[i][j][k],1.0 / 6,buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],jello->v[i][j][k],jello->v[i][j][k]);
      }

  return;  
}
