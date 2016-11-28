#include "comms.h"

#define CONTROLREFRESH 25

ControlPacket CPacket;

unsigned long controlTime, controlTimeOld;

void controls() {
  if (Connected) {

#if 0
    if (digitalRead(SASPIN))  //--------- This is how you do main controls
      MainControls(SAS, HIGH);
    else
      MainControls(SAS, LOW);

    if (digitalRead(RCSPIN))
      MainControls(RCS, HIGH);
    else
      MainControls(RCS, LOW);

    if (digitalRead(CG1PIN))   //--------- This is how you do control groups
      ControlGroups(1, HIGH);
    else
      ControlGroups(1, LOW);      

    //This is an example of reading analog inputs to an axis, with deadband and limits
    CPacket.Throttle = constrain(map(analogRead(THROTTLEPIN),THROTTLEDB,1024-THROTTLEDB,0,1000),0, 1000);
#endif

    KSPBoardSendData(details(CPacket));
  }
}

void output()
{
  unsigned long now = millis();
  controlTime = now - controlTimeOld; 
  if (controlTime > CONTROLREFRESH){
    controlTimeOld = now;
    controls();
  }    
}

#if 0
void controlsInit() {
  pinMode(SASPIN, INPUT_PULLUP);
  pinMode(RCSPIN, INPUT_PULLUP);
  pinMode(CG1PIN, INPUT_PULLUP);
}

void MainControls(byte n, boolean s) {
  if (s)
    CPacket.MainControls |= (1 << n);       // forces nth bit of x to be 1.  all other bits left alone.
  else
    CPacket.MainControls &= ~(1 << n);      // forces nth bit of x to be 0.  all other bits left alone.
}

void ControlGroups(byte n, boolean s) {
  if (s)
    CPacket.ControlGroup |= (1 << n);       // forces nth bit of x to be 1.  all other bits left alone.
  else
    CPacket.ControlGroup &= ~(1 << n);      // forces nth bit of x to be 0.  all other bits left alone.
}
#endif


