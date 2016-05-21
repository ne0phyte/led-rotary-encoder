#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "light_ws2812.h"
#include "twi.h"

#define true  1
#define false 0

volatile int8_t knob_pos;
volatile int8_t knob_posExt;
volatile int8_t knob_old;

#define KNOB     (PINA & 0b11)
#define BUTTON   ((PINA >> 2) & 1)

static const int8_t KNOB_DIRECTION[] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
   -1,  0,  0,  1,
   0,  1, -1,  0  };


ISR(PCINT0_vect) {
   uint8_t state = KNOB;
   if (knob_old != state ) {
      knob_pos += KNOB_DIRECTION[state | (knob_old << 2)];
      if (state == 3) {
         knob_posExt = knob_pos >> 2;
      }
      knob_old = state;
   }
};

#define LED_COUNT 12
struct cRGB leds[LED_COUNT];

uint8_t brightness = 20;
uint8_t wheelPos;

//void Wheel(uint8_t pos, uint8_t led) {
//   if (pos < 85) {
//      leds[led].r = pos * 3;
//      leds[led].g = 255 - pos * 3;
//      leds[led].b = 0;
//   } else if (pos < 170) {
//      pos -= 85;
//      leds[led].r = 255 - pos * 3;
//      leds[led].g = 0; 
//      leds[led].b = pos * 3;
//   } else {
//      pos -= 170;
//      leds[led].r = 0; 
//      leds[led].g = pos * 3;
//      leds[led].b = 255 - pos * 3;
//   }
//}
//void updateBrightness(uint8_t led) {
//   leds[led].r = (brightness * leds[led].r) / 255;
//   leds[led].g = (brightness * leds[led].g) / 255;
//   leds[led].b = (brightness * leds[led].b) / 255;
//}
//
//void updateWheel(void) {
//   uint8_t i;
//   for (i = 0; i < LED_COUNT; i++) {
//      Wheel(wheelPos+ (i*22), i);
//      updateBrightness(i);
//   }
//   wheelPos+=2;
//}

void twi_on_request(TWIMsg *msg) {
   // send 3 byte message
   TWI_MSG4(msg, 
         brightness, 
         leds[0].r,
         leds[0].g,
         leds[0].b);
}

void twi_on_receive(TWIMsg *msg) {
   uint8_t i = msg->data[0];

   switch (msg->len) {
      case 2: // set brightness
         if (i & 1) {
            brightness = msg->data[1];
         }
         break;
      case 4: // set led color
         leds[i].r = msg->data[1];
         leds[i].g = msg->data[2];
         leds[i].b = msg->data[3];
         break;
   }
}

int main (void) {
   CCP = 0xD8;
   CLKPSR = 0;

   DDRA &= ~(_BV(PA0) | _BV(PA1) | _BV(PA2));
   PORTA = _BV(PA0) | _BV(PA1) | _BV(PA2);

   twi_init(0x42);
   // interrupts
   PCMSK0 = _BV(PCINT0) | _BV(PCINT1) | _BV(PCINT2);
   sei();
   while(true) {
      //updateWheel();
      //updateBrightness(brightness);
      ws2812_setleds(leds, LED_COUNT);
      _delay_ms(16);
   }
   return 0;
}
