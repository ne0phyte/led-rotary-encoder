#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "light_ws2812.h"

#define true  1
#define false 0

volatile int8_t knob1_pos;
volatile int8_t knob2_pos;
volatile int8_t knob1_posExt;
volatile int8_t knob2_posExt;
volatile int8_t knob1_old;
volatile int8_t knob2_old;

#define KNOB1     ((PINA >> 3) & 0b11)
#define BUTTON1   ((PINA >> 2) & 1)

#define KNOB2     ( ((PINB >> 2) & 0b10 ) | ((PINB >> 1) & 1) )
#define BUTTON2   (PINB & 1)

static const int8_t KNOB_DIRECTION[] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
  -1,  0,  0,  1,
  0,  1, -1,  0  };


ISR(PCINT0_vect) {
  uint8_t state = KNOB1;
  if (knob1_old != state ) {
    knob1_pos += KNOB_DIRECTION[state | (knob1_old << 2)];
    if (state == 3) {
      knob1_posExt = knob1_pos >> 2;
    }
    knob1_old = state;
  }
};

ISR(PCINT1_vect) {
  uint8_t state = KNOB2;
  if (knob2_old != state ) {
    knob2_pos += KNOB_DIRECTION[state | (knob1_old << 1)];
    if (state == 3) {
      knob2_posExt = knob2_pos >> 2;
    }
    knob2_old = state;
  }
};

#define WS2812_LOW_NS      400 // 3*125 = 375 
#define WS2812_HIGH_NS     900 // 7*125 = 875
#define WS2812_PERIOD_NS   1250// 10*125 = 1250

#define NOP_NS (1000/(F_CPU/1000000))
#define NOP asm volatile("nop")

//inline void delay_ns(uint16_t ns) {
//   for (; ns >= NOP_NS; ns-=NOP_NS) NOP;
//}

#define LED_COUNT 10
struct cRGB leds[LED_COUNT];

void setLeds(void) {
   ws2812_sendarray((uint8_t*)leds, LED_COUNT*3);
};


int main (void) {
   // knob1 pins
   DDRA = _BV(PA2) | _BV(PA3) | _BV(PA4);
   PORTA = _BV(PA2) | _BV(PA3) | _BV(PA4);

   // knob2 pins
   DDRB = _BV(PB0) | _BV(PB1) | _BV(PB3);
   PORTB = _BV(PB0) | _BV(PB1) | _BV(PB3);

   // interrupts
   PCMSK0 = _BV(PCINT4) | _BV(PCINT3) | _BV(PCINT2);
   PCMSK1 = _BV(PCINT11) | _BV(PCINT9) | _BV(PCINT8);

   while(true) {
      setLeds();
   }
   return 0;
}
