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

uint8_t hexCharToInt(char pChar) {
    // If character is lowercase (i.e. ASCII value greater or equal 97), "convert"
    // it to uppercase
    if (pChar >= 97) {
        pChar -= 32;
    }
    
    // If character is a letter (i.e. ASCII value greater or equal 65), substract
    // a certain amount so that 'A' would be mapped to 10
    if (pChar >= 65) {
        pChar -= 55;
        return pChar < 16 ? pChar : 0;
    } else {
        // Otherwise map '0' to 0
        pChar -= 48;
        return pChar < 10 ? pChar : 0;
    }
        
    return 0;
}
