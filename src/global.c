/**
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
	    mail@k621.de
      www.k621.de

  Global benötigte Methoden und Strukturen

  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#include "global.h"

void _delay_s(uint8_t pSeconds) {
  for(pSeconds = pSeconds * 4; pSeconds > 0; pSeconds--) {
    _delay_ms(250);
  }
}

uint8_t strStartsWith(const char *pString, char *pPattern) {
    uint8_t i = 0;
    while(pPattern[i]) {
        if(!(pString[i] == pPattern[i])) {
            return FALSE;
        }
        i++;
    }
    return TRUE;
}
