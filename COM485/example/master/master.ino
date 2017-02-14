#define RS_SERIAL Serial3#define RS_BAUDRATE 115200
#define RS_ENABLE_PIN 3
#define RS_TIMEOUT 100
#define RS_MYADRESS 0
#define RS_MAXTIMESEND 5

#include <COM485.h>

void setup() {
  RS485_init();
}

void loop() {
  byte toAdress = 1;
  int ret = RS485_Requestfrom(toAdress,"This is your master"); //stuurt data
  handleIt(ret,toAdress);
    
  delay(3000);
}


void handleIt(byte ret, int RSNUM) {

  switch (ret) {

  case RS_BAD_DATA: //slave doesn't react; or no startbyte is found (header corrupt)
      Serial.println(" Sklave " + String(RSNUM) + ": >> ist verruckt oder arbeitet nicht!!" + String(millis()));

    break;
    
  case RS_BAD_CRC: //data isn't received right for RS_MAXTIMESEND times.
    Serial.print("BADCRC");
    break;
    
  case RS_OK: //message is received right
    Serial.println("From Device: " + String(RS_RET.FromAdress));
    Serial.println("Datalength: " + String(RS_RET.dataLength));
    Serial.println("Data: " + RS_RET.retData);  
  }
}
