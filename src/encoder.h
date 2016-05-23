#ifndef ENCODER_H_
#define ENCODER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define KNOB   (PINA & 0b11)
#define BUTTON ((PINA >> 2) & 1)

typedef struct Encoder {
   volatile int16_t position;
   volatile uint8_t button;
   volatile uint8_t last;
} Encoder;

extern Encoder encoder;

void encoder_init(void);
#endif
