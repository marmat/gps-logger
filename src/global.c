/**
 * \file global.c
 * \brief methods that may be used in various areas of the project
 * \author Martin Matysiak
 */

#include "global.h"

void _delay_s(uint8_t pSeconds) {
    for(pSeconds = pSeconds * 4; pSeconds > 0; pSeconds--) {
        _delay_ms(250);
    }
}
