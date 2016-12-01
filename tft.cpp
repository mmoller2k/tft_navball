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

#define TFT_GREY 0x7BEF

uint32_t runTime = 0;
TSphere3D sphere3D[2];
const int resSphere[]={8,8,10,12,16,16,16,12,10,8,8};
static int SphereVis=0;
unsigned long tLast, tNow;
extern boolean Connected;

void showParam(void);
void drawSphere(int alpha, int beta, int gamma);
void navPtr(void);
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

void drawSphere(int alpha, int beta, int gamma)
{
    double phi, lambda;
    int i,ii,j,jj,x0,y0,x1,y1;
    int res=ROUND;
    static int lc=0;
    int s = lc&1;

    //tft.fillRect(0, 0, 480, 320, TFT_BLACK);
    lc++;
    if(!(lc%4))tft.drawCircle(160,160,124,TFT_LIGHTGREY);
    unsigned color=TFT_WHITE;

    SphereVis = !SphereVis;
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
      if(i==LATS/2)color=TFT_WHITE;
      else if(i<LATS/2)color=TFT_ORANGE;
      else color=TFT_CYAN;
      //res = resSphere[i];
      ii=i+1;
      for(j=0;j<res;j++){
        SerialHelper();
        jj=(j+1)%res;
        
        if(sphere3D[!SphereVis].p[i][j].z>0){
          x0 = sphere3D[!SphereVis].p[i][j].x;
          y0 = sphere3D[!SphereVis].p[i][j].y;
          x1 = sphere3D[!SphereVis].p[i][jj].x;
          y1 = sphere3D[!SphereVis].p[i][jj].y;
          tft.drawLine(x0,y0,x1,y1,TFT_BLACK);
          if(ii<LATS){// && (j%(ROUND/4))==0){
            x0 = sphere3D[!SphereVis].p[i][j].x;
            y0 = sphere3D[!SphereVis].p[i][j].y;
            x1 = sphere3D[!SphereVis].p[ii][j].x;
            y1 = sphere3D[!SphereVis].p[ii][j].y;
            if((j%(ROUND/4))==0){
              tft.drawLine(x0,y0,x1,y1,TFT_BLACK);
            }
            tft.drawRect((x0+x1)/2-1,(y0+y1)/2-1,2,2,TFT_BLACK);
          }
        }

        if(sphere3D[SphereVis].p[i][j].z>0){
          x0 = sphere3D[SphereVis].p[i][j].x;
          y0 = sphere3D[SphereVis].p[i][j].y;
          x1 = sphere3D[SphereVis].p[i][jj].x;
          y1 = sphere3D[SphereVis].p[i][jj].y;
          tft.drawLine(x0,y0,x1,y1,color);
          if(ii<LATS){// && (j%(ROUND/4))==0){
            x0 = sphere3D[SphereVis].p[i][j].x;
            y0 = sphere3D[SphereVis].p[i][j].y;
            x1 = sphere3D[SphereVis].p[ii][j].x;
            y1 = sphere3D[SphereVis].p[ii][j].y;
            if((j%(ROUND/4))==0){
              tft.drawLine(x0,y0,x1,y1,TFT_WHITE);
            }
            if(i<LATS/2){
              tft.drawRect((x0+x1)/2-1,(y0+y1)/2-1,2,2,TFT_ORANGE);
            }
            else{
              tft.drawRect((x0+x1)/2-1,(y0+y1)/2-1,2,2,TFT_CYAN);
            }
          }
        }
      }
    }
    navPtr();
    ballvals();
}

void navPtr(void)
{
  tft.drawLine(140,160,152,160,TFT_YELLOW);
  tft.drawLine(152,160,160,168,TFT_YELLOW);
  tft.drawLine(168,160,180,160,TFT_YELLOW);
  tft.drawLine(168,160,160,168,TFT_YELLOW);
  tft.drawPixel(160,160,TFT_YELLOW);
}

void ballvals(void)
{
  int i,j;
  int x0,y0;
  int yaw[]={0,90,180,270};
  int pit[]={80,60,40,20,0,20,40,60,80};
  for(i=0;i<ROUND;i+=(ROUND/4)){
    SerialHelper();
    if(sphere3D[!SphereVis].p[4][i].z>0){
      x0 = sphere3D[!SphereVis].p[4][i].x;
      y0 = sphere3D[!SphereVis].p[4][i].y;
      tft.setTextColor(TFT_BLACK);
      tft.drawNumber(yaw[i/(ROUND/4)],x0,y0,2);
    }
    if(sphere3D[SphereVis].p[4][i].z>0){
      x0 = sphere3D[SphereVis].p[4][i].x;
      y0 = sphere3D[SphereVis].p[4][i].y;
      tft.setTextColor(TFT_WHITE);
      tft.drawNumber(yaw[i/(ROUND/4)],x0,y0,2);
    }
  }
  for(j=0;j<LATS;j++){
    for(i=2;i<ROUND;i+=(ROUND/4)){
      SerialHelper();
      if(sphere3D[!SphereVis].p[j][i].z>0){
      x0 = sphere3D[!SphereVis].p[j][i].x;
      y0 = sphere3D[!SphereVis].p[j][i].y;
      tft.setTextColor(TFT_BLACK);
      tft.drawNumber(pit[j],x0,y0,2);
      }
      if(sphere3D[SphereVis].p[j][i].z>0){
      x0 = sphere3D[SphereVis].p[j][i].x;
      y0 = sphere3D[SphereVis].p[j][i].y;
      tft.setTextColor(TFT_LIGHTGREY);
      tft.drawNumber(pit[j],x0,y0,2);
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



