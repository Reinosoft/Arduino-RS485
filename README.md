# Arduino-RS485
RS485 Module with CRC support and resend function.
With this module it's possible to talk between master and slaves and check if the message is received without errors.
When the message isn't received right; the data is resend RS_MAXTIMESEND; 

from the <B>MASTER</B>
  use <B>int RS485_Requestfrom(toAdress,String Message)</B> to send a message to a slave and receive data from the slave.
  the function returns:
    RS_BAD_DATA: //slave doesn't react; or no startbyte is found (header corrupt)
    RS_BAD_CRC: //data isn't received right for RS_MAXTIMESEND times. 
    RS_OK: //message is received right 

from the <B>SLAVE</B>
use <B> int ret = RS485_Receivedata()</B> to receive the message; which is sended by the slave.
 
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
