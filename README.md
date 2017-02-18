# Arduino-RS485
RS485 Module with CRC support and resend function.

With this module it's possible to talk between master and slaves and check if the message is received without errors.
It receives the message, and does a CRC check. 
When this failes the device returns a errorflag which allows the sourcedevice to resend the message.
This repeatcount sequence is variable with RS_MAXTIMESEND

<B>How to use:</B> 

<B>MASTER</B>
  use <B>int RS485_Requestfrom(toAdress,String Message)</B> to send a message to a slave and receive data from the slave.
  the function returns:
    RS_BAD_DATA: //slave doesn't react; or no startbyte is found (header corrupt)
    RS_BAD_CRC: //data isn't received right for RS_MAXTIMESEND times. 
    RS_OK: //message is received correctly 

<B>SLAVE</B>
use <B> int ret = RS485_Receivedata()</B> to receive the message; which is sended by the slave.
the function returns:
  RS_BAD_DATA: //no header startbyte found
  RS_BAD_CRC: //maxtimesend gepasseerd, it took to long to receive a message
  RS_NOT_FORME: //the received data is for another slave / master ID
  RS_OK: //message is received correctly
 
