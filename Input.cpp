#include "comms.h"

//if no message received from KSP for more than 2s, go idle
#define IDLETIMER 4000

unsigned long deadtime, deadtimeOld;

typedef unsigned char byte;

HandShakePacket HPacket;
VData_s VData;

// Handshake
void Handshake(){
  //digitalWrite(GLED,HIGH); 

  HPacket.id = 0;
  HPacket.M1 = 3;
  HPacket.M2 = 1;
  HPacket.M3 = 4;

  KSPBoardSendData(details(HPacket));
  //Serial.println(F("KSP;0"));  
  //delay(1000);
}

int input()
{
  int returnValue = -1;
  unsigned long now = millis();

  if (KSPBoardReceiveData()){
    deadtimeOld = now;
    returnValue = id;
    switch(id) {
    case 0: //Handshake packet
      Handshake();
      break;
    case 1:
      //Indicators();
      break;
    }

    //We got some data, turn the green led on
    //digitalWrite(GLED,HIGH);
    Connected = true;
  }
  else
  { //if no message received for a while, go idle
    deadtime = now - deadtimeOld; 
    if (deadtime > IDLETIMER)
    {
      deadtimeOld = now;
      Handshake();
      Connected = false;
      //LEDSAllOff();
    }    
  }

  return returnValue;
}

byte ControlStatus(byte n)
{
  return ((VData.ActionGroups >> n) & 1) == 1;
}

