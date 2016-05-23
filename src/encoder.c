#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "encoder.h"

uint8_t gray_to_dec[4] = {0,1,3,2};
Encoder encoder;

void encoder_init(void) {
   DDRA &= ~(_BV(PA0) | _BV(PA1) | _BV(PA2));
   PUEA |= _BV(PUEA0) | _BV(PUEA1) | _BV(PUEA2);
   PCMSK0 |= _BV(PCINT0) | _BV(PCINT1) | _BV(PCINT2);
   GIMSK |= _BV(PCIE0);
   encoder.last = gray_to_dec[KNOB];
}

ISR(PCINT0_vect) {
   int8_t tmp = gray_to_dec[KNOB];
   int8_t diff = (tmp - encoder.last);
   encoder.last = tmp;
   if (diff == 1 || diff == -1) {
      encoder.position += diff;
   }
   if (BUTTON) {
      encoder.button = 1;
   }
}

