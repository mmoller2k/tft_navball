#include "comms.h"
#include "tft.h"

int lc=0;
void setup() {
  Serial.begin(38400);
  tft_setup();
  InitTxPackets();
}

void loop()
{
  lc++;
  if(input()==1){
    tft_loop();
  }
  else{
    //tft_loop();
    showParam();
  }
  output();
  //if(!(lc%100))SerialUSB.println(lc);
}

