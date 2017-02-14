//{RS485

#define RS_SERIAL Serial3
#define RS_BAUDRATE 115200
#define RS_ENABLE_PIN 3
#define RS_TIMEOUT 100
#define RS_MYADRESS 1
#define RS_MAXTIMESEND 5

#include <COM485.h>

void setup() {
  RS485_init();
}

void loop() {
  HandleRSData();
}
void HandleRSData() {
  /* every RS_MYADRESS > 0 functions as a slave
   *  the master sends data to a slave 
   *  the first thing the slave must do when RS_OK is triggered
   *  is sending data back.
   *  The Master is waiting RS_TIMEOUT for receiving data;
   *  When no data is returned; the master resends the data for RS_MAXTIMESEND
   */
  
 int ret = RS485_Receivedata();

  switch (ret) {
  case RS_BAD_DATA: //no header startbyte found
    Serial.println("BAD DATA");
    break;
    
  case RS_BAD_CRC: //maxtimesend gepasseerd
    Serial.println("BAD CRC");
    break;
    
  case RS_NOT_FORME: //the received data is for another device ID
    Serial.println("From Device: " + String(RS_RET.FromAdress));
    Serial.println("To Device: " + String(RS_RET.ToAdress));
    Serial.println("Datalength: " + String(RS_RET.dataLength));
    Serial.println("Data: " + RS_RET.retDataNotForMe);
    break;
    
  case RS_OK: //bericht goed ontvangen.
    RS485_Senddata(RS_RET.FromAdress, "Dies ist eine Nachricht von dem Sklave");
    Serial.println("From Device: " + String(RS_RET.FromAdress));
    Serial.println("Datalength: " + String(RS_RET.dataLength));
    Serial.println("Data: " + RS_RET.retData);  }

    
 }
