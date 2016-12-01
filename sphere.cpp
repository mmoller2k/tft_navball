#include <stdio.h>
#include <math.h>
#include "sphere.h"

/*
  Alfa: Integer;   //Rotation around X axis
  Beta: Integer;   //Rotation around Y axis
  C: TPoint;       //Center
  R: Integer;      //Radius
  Phi: Integer;    //Angle relative to XY plane
  Lambda: Integer; //Angle around Z axis (from pole to pole)
  P: TPoint3D;     //2D projection of a 3D point on the sphere's surface
*/

#define Sin(x) sini(x)
#define Cos(x) Sin(x+90.0)
#define TRIGSCALE 7
#define TRIGRES 128

int sint[91];

void sin_init(void)
{
  int i;
  for(i=0;i<=90;i++){
    sint[i] = sin(degtorad(i)) * TRIGRES;
  }
}

int sini(int phi)
{
  int r;
  while(phi<0)phi+=360;
  while(phi>=360)phi-=360;
  if(phi<90)r = sint[phi];
  else if(phi<180)r = sint[180-phi];
  else if(phi<270)r = -sint[phi-180];
  else r = -sint[360-phi];
  return r;
}

static TPoint3D Sphere(int phi, int lambda)
{
    TPoint3D q;
    q.x = Cos(phi) * Sin(lambda) / TRIGRES;
    q.y = -Sin(phi);
    q.z = Cos(phi) * Cos(lambda) / TRIGRES;
    return q;
}

static TPoint3D RotX(const TPoint3D p, int alpha)
{
    TPoint3D q;
    q.x = p.x;
    q.y = (p.y * Cos(alpha) + p.z * Sin(alpha)) / TRIGRES;
    q.z = (-p.y * Sin(alpha) + p.z * Cos(alpha)) / TRIGRES;
    return q;
}

static TPoint3D RotY(const TPoint3D p, int beta)
{
    TPoint3D q;
    q.x = (p.x * Cos(beta) + p.z * Sin(beta)) / TRIGRES;
    q.y = p.y;
    q.z = (-p.x * Sin(beta) + p.z * Cos(beta)) / TRIGRES;
    return q;
}

static TPoint3D RotZ(const TPoint3D p, int gamma)
{
    TPoint3D q;
    q.x = (p.x * Cos(gamma) + p.y * Sin(gamma)) / TRIGRES;
    q.y = (-p.x * Sin(gamma) + p.y * Cos(gamma)) / TRIGRES;
    q.z = p.z;
    return q;
}

//#define scale(x) (x/(TRIGRES>>7)+160)
#define scale(x) (x+160)
TPoint3D SPoint(int phi, int lambda, int alpha, int beta, int gamma)
{
    TPoint3D q;
    q = Sphere(phi, lambda);
    q = RotY(q, beta);
    q = RotX(q, alpha);
    q = RotZ(q, gamma);
    //q.x = scale(q.x);
    //q.y = scale(q.y);
    return q;
}


