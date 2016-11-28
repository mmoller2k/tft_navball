#include "comms.h"
#include "tft.h"

void setup() {
  Serial.begin(38400);
  tft_setup();
  InitTxPackets();
}

void loop()
{
  if(input()==1){
    tft_loop();
  }
  else{
    showParam();
  }
  output();
}


















