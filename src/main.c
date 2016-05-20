#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "light_ws2812.h"

#define true  1
#define false 0

volatile int8_t knob_pos;
volatile int8_t knob_posExt;
volatile int8_t knob_old;

#define KNOB     (PINA & 0b11)
#define BUTTON   ((PINA >> 2) & 1)

//  static const int8_t KNOB_DIRECTION[] = {
//    0, -1,  1,  0,
//    1,  0,  0, -1,
//    -1,  0,  0,  1,
//    0,  1, -1,  0  };
//  
//  
//  ISR(PCINT0_vect) {
//    uint8_t state = KNOB;
//    if (knob_old != state ) {
//      knob_pos += KNOB_DIRECTION[state | (knob_old << 2)];
//      if (state == 3) {
//        knob_posExt = knob_pos >> 2;
//      }
//      knob_old = state;
//    }
//  };
//  
//  #define WS2812_LOW_NS      400 // 3*125 = 375 
//  #define WS2812_HIGH_NS     900 // 7*125 = 875
//  #define WS2812_PERIOD_NS   1250// 10*125 = 1250
//  
//  #define NOP_NS (1000/(F_CPU/1000000))
//  #define NOP asm volatile("nop")
//  
//  //inline void delay_ns(uint16_t ns) {
//  //   for (; ns >= NOP_NS; ns-=NOP_NS) NOP;
//  //}

#define LED_COUNT 12
struct cRGB leds[LED_COUNT];

void Wheel(uint8_t pos, uint8_t led) {
  if (pos < 85) {
    leds[led].r = pos * 3;
    leds[led].g = 255 - pos * 3;
    leds[led].b = 0;
  } else if (pos < 170) {
    pos -= 85;
    leds[led].r = 255 - pos * 3;
    leds[led].g = 0; 
    leds[led].b = pos * 3;
  } else {
    pos -= 170;
    leds[led].r = 0; 
    leds[led].g = pos * 3;
    leds[led].b = 255 - pos * 3;
  }
}

uint8_t wheel_pos;
void updateWheel(void) {
   uint8_t i;
   for (i = 0; i < LED_COUNT; i++) {
      Wheel(wheel_pos + (i*22), i);
   }
   wheel_pos+=2;
}

void updateBrightness(uint8_t val) {
   uint8_t i;
   for (i = 0; i < LED_COUNT; i++) {
      leds[i].r = (val * leds[i].r) / 255;
      leds[i].g = (val * leds[i].g) / 255;
      leds[i].b = (val * leds[i].b) / 255;
   };
}

int main (void) {
   CCP = 0xD8;
   CLKPSR = 0;
   
   DDRA &= ~(_BV(PA0) | _BV(PA1) | _BV(PA2));
   PORTA = _BV(PA0) | _BV(PA1) | _BV(PA2);

   // interrupts
   //PCMSK0 = _BV(PCINT0) | _BV(PCINT1) | _BV(PCINT2);

   while(true) {
      updateWheel();
      updateBrightness(100);
      ws2812_setleds(leds, LED_COUNT);
      _delay_ms(1);
   }
   return 0;
}
