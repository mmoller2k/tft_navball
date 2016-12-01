#ifndef _sphere_h_
#define _sphere_h_

#define degtorad(a) ((a*M_PI/180.0))
#define LATS 9
#define ROUND 16
#define XOFFS 160
#define YOFFS 160

typedef struct{
    int x;
    int y;
    int z;
}TPoint3D;

typedef struct{
  TPoint3D p[LATS][ROUND];
}TSphere3D;

TPoint3D SPoint(int phi, int lambda, int alpha, int beta, int gamma);
void sin_init(void);

#endif

