// Demo based on:
// UTFT_Demo by Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
/*

 The delay between tests is set to 0. The tests run so fast you will need to
 change the WAIT value below to see what is being plotted!
 
 This sketch uses the GLCD and font 2 only.

 Make sure all the required fonts are loaded by editting the
 User_Setup.h file in the TFT_ILI9341 library folder.

 If using an UNO or Mega (ATmega328 or ATmega2560 processor) then for best
 performance use the F_AS_T option found in the User_Setup.h file in the
 TFT_ILI9341 library folder.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  ######            TO SELECT THE FONTS YOU USE, SEE ABOVE           ######
  #########################################################################
 */

// Delay between demo pages
//#define WAIT 2000 // Delay between tests, set to 0 to demo speed, 2000 to see what it does!

//#define CENTRE 240

#define BOARD_DUE

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

void showParam(void);
void drawSphere(int alpha, int beta, int gamma);
void fillSphere(void);
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
  drawSphere(0, 0, 0);
  fillSphere();
  tLast=millis();
}

void tft_loop()
{
  static int a=0,b=0,c=0;

  showParam();
  tft.setTextColor(TFT_LIGHTGREY,TFT_BLACK);
  tft.drawNumber(1000/(tNow-tLast), 0,0,2); //FPS
  tLast=tNow;
  tNow=millis();
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

void fillSphere(void)
{
  int i,ii,j,jj,x0,y0,x1,y1,x2,y2,x3,y3;
  unsigned fcolor;
  for(i=0;i<LATS-1;i++){
    if(i<LATS/2)fcolor=TFT_ORANGE;
    else fcolor=TFT_CYAN;

    ii=i+1;
    for(j=0;j<ROUND;j++){
      jj=(j+1)%ROUND;
      
      if(sphere3D[SphereVis].p[i][j].z>0){
        x0 = sphere3D[SphereVis].p[i][j].x;
        y0 = sphere3D[SphereVis].p[i][j].y;
        x1 = sphere3D[SphereVis].p[i][jj].x;
        y1 = sphere3D[SphereVis].p[i][jj].y;
        x2 = sphere3D[SphereVis].p[ii][j].x;
        y2 = sphere3D[SphereVis].p[ii][j].y;
        x3 = sphere3D[SphereVis].p[ii][jj].x;
        y3 = sphere3D[SphereVis].p[ii][jj].y;

        tft.fillTriangle(x0,y0,x1,y1,x2,y2,fcolor);
        tft.fillTriangle(x3,y3,x1,y1,x2,y2,fcolor);
      }
    }
  }  
}

void drawSphere(int alpha, int beta, int gamma)
{
    double phi, lambda;
    int i,ii,j,jj;
    int x0,y0,x1,y1,x2,y2,x3,y3, z0,z1,z2,z3;
    int res=ROUND;

    //tft.fillRect(0, 0, 480, 320, TFT_BLACK);
    unsigned scolor=TFT_CYAN;

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

    for(i=0;i<LATS;i++){
      /* erase old sphere */
      if(i<LATS/2)scolor=TFT_ORANGE;
      else scolor=TFT_CYAN;

      //res = resSphere[i];
      ii=i+1;
      for(j=0;j<res;j++){
        SerialHelper();
        jj=(j+1)%res;
        
        if(sphere3D[!SphereVis].p[i][j].z>0 && sphere3D[SphereVis].p[i][jj].z>0){
          x0 = sphere3D[!SphereVis].p[i][j].x;
          y0 = sphere3D[!SphereVis].p[i][j].y;
          x1 = sphere3D[!SphereVis].p[i][jj].x;
          y1 = sphere3D[!SphereVis].p[i][jj].y;
          x2 = sphere3D[!SphereVis].p[ii][j].x;
          y2 = sphere3D[!SphereVis].p[ii][j].y;
          x3 = sphere3D[!SphereVis].p[ii][jj].x;
          y3 = sphere3D[!SphereVis].p[ii][jj].y;
          /* erase latitude lines */
          if(i!=4)tft.drawLine(x0,y0,x1,y1,scolor);

          if(ii<LATS){// && (j%(ROUND/4))==0){
            x0 = sphere3D[!SphereVis].p[i][j].x;
            y0 = sphere3D[!SphereVis].p[i][j].y;
            x1 = sphere3D[!SphereVis].p[ii][j].x;
            y1 = sphere3D[!SphereVis].p[ii][j].y;
            if((j%(ROUND/4))==0){
              /* erase longitude lines */
              tft.drawLine(x0,y0,x1,y1,scolor);
            }
            /* erase marker points */
            tft.drawRect((x0+x1)/2-1,(y0+y1)/2-1,2,2,scolor);
          }
        }
      }
      /* draw new sphere */
      for(j=0;j<res;j++){
        SerialHelper();
        jj=(j+1)%res;

        z0 = sphere3D[SphereVis].p[i][j].z;
        z1 = sphere3D[SphereVis].p[i][jj].z;
        z2 = sphere3D[SphereVis].p[ii][j].z;
        z3 = sphere3D[SphereVis].p[ii][jj].z;
        x0 = sphere3D[SphereVis].p[i][j].x;
        y0 = sphere3D[SphereVis].p[i][j].y;
        x1 = sphere3D[SphereVis].p[i][jj].x;
        y1 = sphere3D[SphereVis].p[i][jj].y;
        x2 = sphere3D[SphereVis].p[ii][j].x;
        y2 = sphere3D[SphereVis].p[ii][j].y;
        x3 = sphere3D[SphereVis].p[ii][jj].x;
        y3 = sphere3D[SphereVis].p[ii][jj].y;
          /* fill only the equator */
        if(i==3 && (z2>0 || z3>0)){
          tft.fillTriangle(x2,y2,x3,y3,x1,y1,TFT_ORANGE);
        }
        if(i==4 && (z0>0 || z1>0)){
          tft.fillTriangle(x0,y0,x1,y1,x2,y2,TFT_CYAN);
        }

        if(z0>0 && z1>0){
          tft.drawLine(x0,y0,x1,y1,TFT_DARKGREY);

/*          
          if(i==4){
            tft.fillTriangle(x0,y0,x1,y1,x2,y2,color);
            tft.fillTriangle(x3,y3,x1,y1,x2,y2,color);
          }
*/          

          if(ii<LATS){// && (j%(ROUND/4))==0){
            x0 = sphere3D[SphereVis].p[i][j].x;
            y0 = sphere3D[SphereVis].p[i][j].y;
            x1 = sphere3D[SphereVis].p[ii][j].x;
            y1 = sphere3D[SphereVis].p[ii][j].y;
            if((j%(ROUND/4))==0){
              if(j==0)tft.drawLine(x0,y0,x1,y1,TFT_MAGENTA);
              else tft.drawLine(x0,y0,x1,y1,TFT_DARKGREY);
            }
            tft.drawRect((x0+x1)/2-1,(y0+y1)/2-1,2,2,TFT_BLACK);
          }
        }
      }
    }
    navPtr(1,1,TFT_BLACK);
    navPtr(0,0,TFT_YELLOW);
    navPtr(0,-1,TFT_YELLOW);
    ballvals();
    //tft.drawCircle(160,160,128,TFT_BLACK);
}

void navPtr(int dx, int dy, unsigned color)
{
  tft.drawLine(140+dx,160+dy,152+dx,160+dy,color);
  tft.drawLine(152+dx,160+dy,160+dx,168+dy,color);
  tft.drawLine(168+dx,160+dy,180+dx,160+dy,color);
  tft.drawLine(168+dx,160+dy,160+dx,168+dy,color);
  tft.drawPixel(160+dx,160+dy,color);
}

#define TEXT_R (110*110)
void ballvals(void)
{
  int i,j;
  int x0,y0;
  long r,xr,yr;
  int yaw[]={0,90,180,270};
  int pit[]={80,60,40,20,0,20,40,60,80};
  unsigned scolor;
  for(i=0;i<ROUND;i+=(ROUND/4)){
    SerialHelper();
/*    
    if(sphere3D[!SphereVis].p[4][i].z>0){
      x0 = sphere3D[!SphereVis].p[4][i].x + XOFFS;
      y0 = sphere3D[!SphereVis].p[4][i].y + YOFFS;
      tft.setTextColor(TFT_ORANGE);
      tft.drawNumber(yaw[i/(ROUND/4)],x0,y0,2);
    }
*/    
    if(sphere3D[SphereVis].p[4][i].z>0){
      x0 = sphere3D[SphereVis].p[4][i].x;
      y0 = sphere3D[SphereVis].p[4][i].y;
      tft.setTextColor(TFT_BLACK);
      tft.drawNumber(yaw[i/(ROUND/4)],x0,y0,2);
      tft.drawNumber(yaw[i/(ROUND/4)],x0-1,y0-1,2);
    }
  }
  for(j=0;j<LATS;j++){
    if(j<LATS/2)scolor=TFT_ORANGE;
    else scolor=TFT_CYAN;

    for(i=3;i<ROUND;i+=(ROUND/4)){
      SerialHelper();
      if(sphere3D[!SphereVis].p[j][i].z>0){
        x0 = sphere3D[!SphereVis].p[j][i].x;
        y0 = sphere3D[!SphereVis].p[j][i].y;
        xr = x0 - XOFFS;
        yr = y0 - YOFFS;
        xr+=6;
        yr+=8;
        r = (xr*xr) + (yr*yr);
        tft.setTextColor(scolor);
        if(j!=4 && r<TEXT_R)tft.drawNumber(pit[j],x0,y0,2);
      }
      if(sphere3D[SphereVis].p[j][i].z>0){
        x0 = sphere3D[SphereVis].p[j][i].x;
        y0 = sphere3D[SphereVis].p[j][i].y;
        xr = x0 - XOFFS;
        yr = y0 - YOFFS;
        xr+=6;
        yr+=8;
        r = (xr*xr) + (yr*yr);
        tft.setTextColor(TFT_DARKGREY);
        if(j!=4 && r<TEXT_R)tft.drawNumber(pit[j],x0,y0,2);
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
  tft.setTextPadding(24);
  tft.drawNumber(VData.Heading,72,32,4);
  tft.drawNumber(VData.Pitch,260,32,4);

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



