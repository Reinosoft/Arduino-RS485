
struct RSRET {
  String LastSended; //Slaat de laatst verstuurde string op; voor hersturen bij CRC fout of whatever
  String retDataNotForMe; //Data ontvangen, welke voor andere slave is.
  String retData; //Data ontvangen, voor dit adres
  int datalength; //Lengte van ontvangen data
  byte retrytime; //Aantal zendpogingen (intern gebruik)
  byte FromAdress; //RS Adres waar data vanaf verzonden is.
  byte ToAdress;  //RS Adres waar data naar verzonden wordt (voor Slave2Slave communicatie)
};

RSRET RS_RET;
//{INIT & MAIN ROUTINES
void RS485_init() {
  RS_SERIAL.begin (RS_BAUDRATE);
  pinMode (RS_ENABLE_PIN, OUTPUT);  // HIGH Enables Write, LOW Enables Read
  digitalWrite(RS_ENABLE_PIN, LOW);
}

int CreateCRC(String DatatoSend) {
  int CRC_val = 0;
  for (int i = 0; i < DatatoSend.length(); i++) {
    int j = DatatoSend.charAt(i);
    CRC_val += DatatoSend.charAt(i);
  }
  return CRC_val;
}

void clearbuffer(bool show) {
    char c;
    while (RS_SERIAL.available()) {
        c = RS_SERIAL.read();
        #ifdef DEBUG
          if (show) {Serial.print(c);}
        #endif
    }

    #ifdef DEBUG
      if (show) {Serial.println();}
    #endif // DEBUG
}
//}

//{ SENDDATA & FIRST PART OF REQUEST
void RS485_Senddata(int ToAdress, String DatatoSend) {

  String myCRC = int2str(CreateCRC(DatatoSend)); //Convert CRC to value en daarna naar string
  char CRC[3] = {myCRC.charAt(0), myCRC.charAt(1), '\0'};
  byte datalength = DatatoSend.length();

  digitalWrite (RS_ENABLE_PIN, HIGH);  //{ enable sending
    RS_SERIAL.write(byte(255));
    RS_SERIAL.write(ToAdress);
    RS_SERIAL.write(RS_MYADRESS);
    RS_SERIAL.write(CRC);
    RS_SERIAL.write(datalength);
    RS_SERIAL.print(DatatoSend);
    RS_SERIAL.flush();

  digitalWrite (RS_ENABLE_PIN, LOW);  //} disable sending
  RS_RET.LastSended = DatatoSend;
}
bool RS485_Requestfrom2(int ToAdress, String DatatoSend) {
  RS485_Senddata(ToAdress, DatatoSend);
  unsigned long MyTimer = millis() + RS_TIMEOUT;

  while (millis() < MyTimer) {
    if (RS_SERIAL.available() >= RS_HEADERLENGTH) { //Data data teruggekomen.
      return true;
    }
  }
  return false;
}

//}

//{RECEIVE DATA
int RS485_Receivedata() {
  //{INIT
  String readedData = "";
  int CRC;

  byte datalength = 0;
  char rawCRC[3];

  byte RS_FromAdress;
  byte RS_Adress = 0;
  //}

  //{READ HEADER

  if (RS_SERIAL.available() >= RS_HEADERLENGTH) {
    byte startbyte = 0;
    while (RS_SERIAL.available()) {
      startbyte = RS_SERIAL.read();
      if (startbyte == 255) {
        break;
      }
    }
    if (startbyte != 255) { //First byte not startbyte
      clearbuffer(true);
      #ifdef DEBUG
        Serial.println("STARTBYTE NOT FOUND");
      #endif // DEBUG
      return RS_BAD_DATA;
    }

    RS_Adress = RS_SERIAL.read();

    RS_FromAdress = RS_SERIAL.read();

    rawCRC[0] = RS_SERIAL.read();
    rawCRC[1] = RS_SERIAL.read();
    rawCRC[2] = '\0';
    CRC = str2int(rawCRC);

    datalength = RS_SERIAL.read();

 //   if(RS_DEBUG > 0) {Serial.println("TO ADRESS: " + String(RS_Adress) + " / FROM ADRESS: " + String(RS_FromAdress) + "CRCVAL: " + String(CRC));}
  }
  else {
    return RS_NOTHING;
  }
  //}

  //{READ DATA
  char c;
  int thisCRC = 0;
  byte readedchars = 0;

  unsigned long MyTimer = millis() + RS_TIMEOUT;

  while (millis() < MyTimer) {

    if (RS_SERIAL.available()) {
      c = RS_SERIAL.read();
      #ifdef DEBUG
        Serial.print(c);
      #endif // DEBUG
      readedData += c;
      readedchars ++;
      thisCRC += c;
    }
    if (readedchars >= datalength) {
      clearbuffer(false);
      break;
    }
  }

  if (readedchars == datalength) { //Data binnengekomen.
    RS_RET.FromAdress = RS_FromAdress;
    RS_RET.datalength = datalength;
    RS_RET.ToAdress = RS_Adress;
    clearbuffer(false); //just in case

    if (thisCRC != CRC) { //CRC NIET OK
      RS_RET.retrytime = 0;
      if (RS_MYADRESS != RS_Adress) {
        return RS_NOT_FORME;
      }
      else {
        return RS_BAD_CRC;
      }
    }
    else { //CRC = OK
      if (RS_MYADRESS != RS_Adress) { //NOTFORME
        RS_RET.retDataNotForMe = readedData;
        RS_RET.retData = "";
        RS_RET.retrytime = 0;
        return RS_NOT_FORME;
      }
      else { //DATA IS VOOR DIT DEVICE
        RS_RET.retDataNotForMe = "";
        RS_RET.retData = readedData;

        if (RS_MYADRESS != 0) { //{ ALS DE MASTER DE BOODSCHAP TERUG NIET GOED ONTVANGEN HEEFT, ZAL DEZE DE DATA NOGMAALS OPVRAGEN.
          if (RS_RET.retData == "NO") { //DATA IS NIET GOED BIJ DE MASTER AANGEKOMEN, DEZE HEEFT NO GEANTWOORD!
              #ifdef DEBUG
                Serial.println("DATA NIET GOED AANGEKOMEN; RESEND");
              #endif
            delay(RS_MUSTHAVEDELAY);
            if (RS_RET.retrytime < RS_MAXTIMESEND) {
              RS485_Senddata(RS_FromAdress, RS_RET.LastSended);
              RS_RET.retrytime ++;
            }
            else {
              RS_RET.retrytime = 0;
            }
            return RS_NOTHING;
          }
        }

        return RS_OK;
      }
    }
  }
}


//{REQUEST
int RS485_Requestfrom(int ToAdress, String DatatoSend) {
  int ret;
  bool sendNO = false;
  for (int i = 0; i < RS_MAXTIMESEND; i++) {
    if (sendNO == true) {
      ret = RS485_Requestfrom2(ToAdress, "NO");
    }
    else {
      ret = RS485_Requestfrom2(ToAdress, DatatoSend);
    }
    if (ret == true) { //Er is data binnengekomen
      ret = RS485_Receivedata();
      if (ret == RS_OK) { //Data goed binnengekomen.
        return ret;
      }
      sendNO = true;
    }
    else { //Er is geen data verzonden door de slave; oorzaak: BAD CRC, GEEN CONTACT...
      ret = RS_BAD_DATA;
    }
  }
  return ret;
}

//}

