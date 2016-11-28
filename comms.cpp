#include "comms.h"

#define BUFSIZE 1024
uint8_t rx_len;
uint8_t * address;
byte rxbuf[BUFSIZE]; //circular receiving buffer
uint8_t structSize;
int rx_array_inx=0, rx_in, rx_out=0, rx_count=0, rx_start;  //index for RX parsing buffer
uint8_t calc_CS;     //calculated Chacksum
boolean Connected;
byte id;
byte c;

void InitTxPackets() {
  HPacket.id = 0;  
  CPacket.id = 101;
}

void SerialHelper(void) // help smooth data receiving
{
  while(Serial.available() && (rx_count < BUFSIZE)){
    rxbuf[rx_in++] = Serial.read();
    rx_in %= BUFSIZE;
    rx_count++;
  }
}

byte BufRead(void)
{
  char c = rxbuf[rx_out];
  if(rx_count>0){
    rx_out++;
    rx_out %= BUFSIZE;
    rx_count--;
  }
  return c;
}

//This shit contains stuff borrowed from EasyTransfer lib
boolean KSPBoardReceiveData() {
  int n;
  SerialHelper();
  if ((rx_len == 0)&&(rx_count>3)){
    while(BufRead()!= 0xBE) {
      if (rx_count == 0)
        return false;  
    }
    if (BufRead() == 0xEF){
      rx_len = BufRead();
      rx_start = rx_out;
      id = BufRead(); 
      rx_array_inx = 1;
      calc_CS = rx_len ^ id;

      switch(id) {
      case 0:
        structSize = sizeof(HPacket);   
        address = (uint8_t*)&HPacket;     
        break;
      case 1:
        structSize = sizeof(VData);   
        address = (uint8_t*)&VData;     
        break;
      }
    }

    //make sure the binary structs on both Arduino's are the same size.
    if(rx_len != structSize){
      rx_len = 0;
      return false;
    }   
  }

  if(rx_len != 0){
    while(rx_count>0 && rx_array_inx <= rx_len){
      c = BufRead();
      calc_CS ^= c;
      rx_array_inx++;
    }

    if(rx_len == (rx_array_inx-1)){
      //seem to have got whole message
      if(calc_CS == 0){//CS good
        n = BUFSIZE - rx_start;
        if(n>=structSize){
          memcpy(address,rxbuf+rx_start,structSize);
        }
        else{ //if packet wraps around circular buffer
          memcpy(address,rxbuf+rx_start,n);
          memcpy(address+n,rxbuf,structSize-n);
        }
        rx_len = 0;
        rx_array_inx = 0;
        return true;
      }
      else{
        //failed checksum, need to clear this out anyway
        rx_len = 0;
        rx_array_inx = 0;
        return false;
      }
    }
  }

  return false;
}

void KSPBoardSendData(uint8_t * address, uint8_t len){
  uint8_t CS = len;
  Serial.write(0xBE);
  Serial.write(0xEF);
  Serial.write(len);
  
  for(int i = 0; i<len; i++){
    CS^=*(address+i);
    Serial.write(*(address+i));
  }
  
  Serial.write(CS);
}
















