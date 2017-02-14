
/* RS485 library met CRC check. Reinald Nijboer (27-09-2016) v2.1

  de master zend een aanvraag naar de slave via --> Requestfrom(RS485_ID, Txt2Send)
  indien er tekst te verzenden is, en deze door de CRCcheck komt, zal de slave antwoorden door data terug te sturen.

  indien er geen antwoord komt, zal de master de data nogmaals versturen.
  na RS_MAXTIMESEND zal er een foutmelding gegeven worden.

  header is opgebouwd als volgt
    BYTE(255) (Startbyte)
    BYTE(RS485 dest adress)
    BYTE(RS485 source adress)
    2*BYTE(CRC_VALUE) // int2str(sum of all ascii values of the data)
    BYTE(DataLen)
    String[DataLen] Data
*/

#include <int2str.h>

#define RS_HEADERLENGTH 6 //lengte v/d Header
#define RS_MUSTHAVEDELAY 10

#define RS_NOTHING 0
#define RS_BAD_CRC 1 //Ontvangen string kommt nicht door de CRC check
#define RS_BAD_DATA 2 //Startbyte niet gevonden, geen idee waar de data wegkomt.
#define RS_NOT_FORME 3 //Er is RS485 data empfangen, maar niet voor dit RS485 adress.
#define RS_OK 4

#include <I2.h>
