// Demo based on:
// UTFT_Demo by Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
/*
  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  ######            TO SELECT THE FONTS YOU USE, SEE ABOVE           ######
  #########################################################################
 */

#include "tft.h"

#define C_GROUND TFT_ORANGE
#define C_SKY TFT_CYAN
#ifdef WIREFRAME
#define CWIRE_EQUATOR TFT_WHITE
#define CWIRE_NORTH TFT_MAGENTA
#define C_SHOW scolor
#define C_ERASE TFT_BLACK
#define CTEXT TFT_LIGHTGREY
#define CTEXT_BOLD TFT_WHITE
#else
#define CWIRE_EQUATOR TFT_BLACK
#define CWIRE_NORTH TFT_MAGENTA
#define C_SHOW TFT_DARKGREY
#define C_ERASE scolor
#define CTEXT TFT_DARKGREY
#define CTEXT_BOLD TFT_BLACK
#endif

#define FILLZONES ((LATS-1)*2)

#ifdef BOARD_DUE
#include <TFT_HX8357_Due.h>
#else
#include <TFT_HX8357.h> // Hardware-specific library
#endif
#include "sphere.h"
#include "comms.h"

#ifdef BOARD_DUE
TFT_HX8357_Due tft = TFT_HX8357_Due();       // Invoke custom library
#else
TFT_HX8357 tft = TFT_HX8357();       // Invoke custom library
#endif

uint32_t runTime = 0;
TSphere3D sphere3D[2];
const int resSphere[]={8,8,10,12,16,16,16,12,10,8,8};
static int SphereVis=0;
unsigned long tLast, tNow;
extern boolean Connected;

void drawSphere(int alpha, int beta, int gamma);
void fillSphere(void);
void fillHelper(int n);
void navPtr(int dx, int dy, unsigned color);
void ballvals(void);

void tft_setup()
{
  //randomSeed(analogRead(0));
  Serial.begin(38400);
// Setup the LCD
  tft.init();
  tft.setRotation(3);
  sin_init();
  tft.fillScreen(TFT_BLACK);
#ifndef WIREFRAME  
  drawSphere(0, 0, 0);
  fillSphere();
#endif  
  drawSphere(0, 0, 0);
  tLast=millis();
}

void tft_loop()
{
  static int a=0,b=0,c=0;
  tLast=tNow;
  tNow=millis();

  tft.setTextColor(TFT_DARKGREY,TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  tft.setTextPadding(40);
  //tft.drawNumber(1000/(tNow-tLast), 40,0,2); //FPS
  tft.drawNumber(tNow-tLast, 40,0,2); //ms
  showParam();
  if(Connected){
    a = VData.Pitch;
    b = -VData.Heading;
    c = -VData.Roll;
  }
  else{
    a++; b+=3;
    a%=360; b%=360;
  }
  drawSphere(a, b, c);
}

void fillHelper(int n)
{
  int e=!(n&1);
  int i=n/2;
  int ii;
  int j,jj;
  int x0,y0,x1,y1,x2,y2,x3,y3;
  unsigned scolor;

  i%=LATS-1;
  ii=i+1;
  
  if(i<LATS/2)scolor=C_GROUND;
  else scolor=C_SKY;
  
  for(j=0;j<ROUND;j++){
    SerialHelper();
    jj=(j+1)%ROUND;
    if(sphere3D[SphereVis].p[i][j].z>8){
      x0 = sphere3D[SphereVis].p[i][j].x;
      y0 = sphere3D[SphereVis].p[i][j].y;
      x1 = sphere3D[SphereVis].p[i][jj].x;
      y1 = sphere3D[SphereVis].p[i][jj].y;
      x2 = sphere3D[SphereVis].p[ii][j].x;
      y2 = sphere3D[SphereVis].p[ii][j].y;
      x3 = sphere3D[SphereVis].p[ii][jj].x;
      y3 = sphere3D[SphereVis].p[ii][jj].y;

      if(e)tft.fillTriangle(x0,y0,x1,y1,x2,y2,scolor);
      else tft.fillTriangle(x3,y3,x1,y1,x2,y2,scolor);
    }
  }
}

void fillSphere(void)
{
  int i;
  for(i=0;i<FILLZONES;i++){
    fillHelper(i);
  }  
}

void drawSphere(int alpha, int beta, int gamma)
{
    double phi, lambda;
    int i,ii,j,jj;
    int x0,y0,x1,y1,x2,y2,x3,y3, z0,z1,z2,z3;
    int res=ROUND;
    static int lc=0;
    static int fill=0;

    lc++;

    //tft.fillRect(0, 0, 480, 320, TFT_BLACK);
    unsigned scolor;

    SphereVis = !SphereVis;
    /* calculator new sphere */
    for(i=0;i<LATS;i++){
        //res = resSphere[i];
        phi = i*20-80;
        for(j=0;j<res;j++){
            SerialHelper();
            lambda = j*360/res;
            sphere3D[SphereVis].p[i][j] = SPoint(phi,lambda,alpha,beta,gamma);
        }
    }
#ifndef WIREFRAME
    fillHelper(7); /* Can't fill the whole thing in one go */
    fillHelper(8);
    fillHelper(fill);
    fillHelper(fill+1);
    fill+=2;
    if(fill==7)fill+=2;
    fill %= FILLZONES;
#endif

    navPtr(1,1,TFT_BLACK);
    navPtr(0,0,TFT_YELLOW);
    navPtr(0,-1,TFT_YELLOW);
    ballvals();

    for(i=0;i<LATS;i++){
      /* erase old sphere */
      if(i<LATS/2)scolor=C_GROUND;
      else scolor=C_SKY;

      //res = resSphere[i];
      ii=i+1;
      for(j=0;j<res;j++){
        SerialHelper();
        jj=(j+1)%res;
        
        x0 = sphere3D[!SphereVis].p[i][j].x;
        y0 = sphere3D[!SphereVis].p[i][j].y;
        z0 = sphere3D[!SphereVis].p[i][j].z;
        x1 = sphere3D[!SphereVis].p[i][jj].x;
        y1 = sphere3D[!SphereVis].p[i][jj].y;
        z1 = sphere3D[!SphereVis].p[i][jj].z;
        x2 = sphere3D[!SphereVis].p[ii][j].x;
        y2 = sphere3D[!SphereVis].p[ii][j].y;
        z2 = sphere3D[!SphereVis].p[ii][j].z;

        if(z0>0 && z1>0){
          /* erase latitude lines */
          tft.drawLine(x0,y0,x1,y1,C_ERASE);

          if(ii<LATS){// && (j%(ROUND/4))==0){
            if((j%(ROUND/4))==0){
              /* erase longitude lines */
              tft.drawLine(x0,y0,x2,y2,C_ERASE);
            }
            /* erase marker points */
            tft.drawPixel((x0+x2)/2-1,(y0+y2)/2,C_ERASE);
            tft.drawPixel((x0+x2)/2+1,(y0+y2)/2,C_ERASE);
            tft.drawPixel((x0+x2)/2,(y0+y2)/2-1,C_ERASE);
            tft.drawPixel((x0+x2)/2,(y0+y2)/2+1,C_ERASE);
          }
        }
      }
      /* draw new sphere */
      for(j=0;j<res;j++){
        SerialHelper();
        jj=(j+1)%res;

        x0 = sphere3D[SphereVis].p[i][j].x;
        y0 = sphere3D[SphereVis].p[i][j].y;
        z0 = sphere3D[SphereVis].p[i][j].z;
        x1 = sphere3D[SphereVis].p[i][jj].x;
        y1 = sphere3D[SphereVis].p[i][jj].y;
        z1 = sphere3D[SphereVis].p[i][jj].z;
        x2 = sphere3D[SphereVis].p[ii][j].x;
        y2 = sphere3D[SphereVis].p[ii][j].y;
        z2 = sphere3D[SphereVis].p[ii][j].z;

        if(z0>0 && z1>0){
          if(i==4)tft.drawLine(x0,y0,x1,y1,CWIRE_EQUATOR);
          else tft.drawLine(x0,y0,x1,y1,C_SHOW);

          if(ii<LATS){// && (j%(ROUND/4))==0){
            x0 = sphere3D[SphereVis].p[i][j].x;
            y0 = sphere3D[SphereVis].p[i][j].y;
            x1 = sphere3D[SphereVis].p[ii][j].x;
            y1 = sphere3D[SphereVis].p[ii][j].y;
            if((j%(ROUND/4))==0){
              if(j==0)tft.drawLine(x0,y0,x2,y2,CWIRE_NORTH);
              else tft.drawLine(x0,y0,x2,y2,C_SHOW);
            }
            tft.drawPixel((x0+x2)/2-1,(y0+y2)/2,C_SHOW);
            tft.drawPixel((x0+x2)/2+1,(y0+y2)/2,C_SHOW);
            tft.drawPixel((x0+x2)/2,(y0+y2)/2-1,C_SHOW);
            tft.drawPixel((x0+x2)/2,(y0+y2)/2+1,C_SHOW);
          }
        }
      }
    }
#ifdef WIREFRAME    
    if(!(lc%6))tft.drawCircle(160,160,124,TFT_LIGHTGREY);
#endif    
}

void navPtr(int dx, int dy, unsigned color)
{
  tft.drawLine(140+dx,160+dy,152+dx,160+dy,color);
  tft.drawLine(152+dx,160+dy,160+dx,168+dy,color);
  tft.drawLine(168+dx,160+dy,180+dx,160+dy,color);
  tft.drawLine(168+dx,160+dy,160+dx,168+dy,color);
  tft.drawPixel(160+dx,160+dy,color);
}

int rText(int x, int y)
{
  int xr = x - XOFFS;
  int yr = y - YOFFS;
  return (xr*xr)+(yr*yr);
}

#define TEXT_R (112*112)
void ballvals(void)
{
  int i,j;
  int x0,y0;
  int yaw[]={0,90,180,270};
  int pit[]={80,60,40,20,0,20,40,60,80};
  unsigned scolor;

  tft.setTextDatum(MC_DATUM);
  for(i=0;i<ROUND;i+=(ROUND/4)){
    SerialHelper();

    for(j=2;j<7;j+=3){
      if(j<LATS/2)scolor=C_GROUND;
      else scolor=C_SKY;
      if(sphere3D[!SphereVis].p[j][i].z>0){
        x0 = (sphere3D[!SphereVis].p[j][i].x + sphere3D[!SphereVis].p[j+1][i].x) / 2;
        y0 = (sphere3D[!SphereVis].p[j][i].y + sphere3D[!SphereVis].p[j+1][i].y) / 2;
        tft.setTextColor(C_ERASE);
        if(rText(x0,y0)<TEXT_R-900)tft.drawNumber(yaw[i/(ROUND/4)],x0,y0,4);
      }
      if(sphere3D[SphereVis].p[j][i].z>0){
        x0 = (sphere3D[SphereVis].p[j][i].x + sphere3D[SphereVis].p[j+1][i].x) / 2;
        y0 = (sphere3D[SphereVis].p[j][i].y + sphere3D[SphereVis].p[j+1][i].y) / 2;
        tft.setTextColor(CTEXT_BOLD);
        if(rText(x0,y0)<TEXT_R-900)tft.drawNumber(yaw[i/(ROUND/4)],x0,y0,4);
      }
    }
  }
  for(j=0;j<LATS;j++){
    if(j<LATS/2)scolor=C_GROUND;
    else scolor=C_SKY;

    for(i=ROUND/8;i<ROUND;i+=(ROUND/4)){
      SerialHelper();
      if(sphere3D[!SphereVis].p[j][i].z>0){
        x0 = sphere3D[!SphereVis].p[j][i].x;
        y0 = sphere3D[!SphereVis].p[j][i].y;
        tft.setTextColor(C_ERASE);
        if(j!=4 && rText(x0,y0)<TEXT_R)tft.drawNumber(pit[j],x0,y0,2);
      }
      if(sphere3D[SphereVis].p[j][i].z>0){
        x0 = sphere3D[SphereVis].p[j][i].x;
        y0 = sphere3D[SphereVis].p[j][i].y;
        tft.setTextColor(CTEXT);
        if(j!=4 && rText(x0,y0)<TEXT_R)tft.drawNumber(pit[j],x0,y0,2);
      }
    }
  }
}

void showParam(void)
{
  static int init=0;
  int m = 340;
  int y=-20,h=40;
  static int lc;
  lc+=17;
  lc %= 10000;

  if(!init){
    init=1;
    tft.setTextColor(TFT_BLUE,TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Ap",m,y+=h,2);
    tft.drawString("to Ap",m,y+=h,2);
    tft.drawString("Pe",m,y+=h,2);
    tft.drawString("to Pe",m,y+=h,2);
    //tft.drawString("Hdg",m,y+=h,2);
    //tft.drawString("Inc",m,y+=h,2);
    tft.drawString("Alt",m,y+=h,2);
    tft.drawString("V",m,y+=h,2);
    tft.drawString("dV",m,y+=h,2);
  }
  y=-2;
  m=408;
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  tft.setTextPadding(80);
  tft.drawNumber(VData.AP,m,y+=h,2);
  tft.drawNumber(VData.TAp,m,y+=h,2);
  tft.drawNumber(VData.PE,m,y+=h,2);
  tft.drawNumber(VData.TPe,m,y+=h,2);
  tft.drawFloat(VData.Alt,1,m,y+=h,2);
  tft.drawFloat(VData.VOrbit,1,m,y+=h,2);
  tft.drawNumber(VData.MNDeltaV,m,y+=h,2);
  tft.setTextPadding(48);
  tft.drawNumber(VData.Heading,72,32,4);
  tft.drawNumber(VData.Pitch,280,32,4);

  tft.setTextDatum(TL_DATUM);
  if(Connected){
    tft.setTextColor(TFT_GREEN,TFT_BLACK);
    tft.drawString("Connected    ",40,300,2);
  }
  else{
    tft.setTextColor(TFT_RED,TFT_BLACK);
    tft.drawString("Connecting...",40,300,2);
  }
}



