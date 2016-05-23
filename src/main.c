#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "light_ws2812.h"
#include "twi.h"
#include "encoder.h"

#define LED_COUNT 12
#define DEFAULT_BRIGHTNESS 50

#define CMD_SET_MODE       (1<<4)
#define CMD_SET_BRIGHTNESS (1<<5)
#define CMD_SET_POSITION   (1<<6) 
#define CMD_SET_LED        (1<<7)
#define CMD_DATA_MASK      0b00001111
#define CMD_MASK           0b11110000

#define MODE_MANUAL        (1<<0)
#define MODE_AUTO          (1<<1)

struct cRGB leds[LED_COUNT];
struct cRGB leds_current[LED_COUNT];

uint8_t brightness = DEFAULT_BRIGHTNESS;
uint8_t manual_mode = 0;

void set_led(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void update_leds(void);
uint8_t animate(uint8_t curr, uint8_t target);
void update_leds_current(void);
void clear_leds(void);
void led_wheel(uint8_t led, uint8_t pos);

void led_wheel(uint8_t led, uint8_t pos) {
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

void twi_on_request(TWIMsg *msg) {
   TWI_MSG1(msg, encoder.position);
}

void twi_on_receive(TWIMsg *msg) {
   uint8_t i = msg->data[0];
    
   switch (i & CMD_MASK) {
      case CMD_SET_MODE:
         switch (i & CMD_DATA_MASK) {
            case MODE_MANUAL:
               clear_leds();
               manual_mode = 1;
               break;
            case MODE_AUTO:
               manual_mode = 0;
               break;
         }
         break;
      case CMD_SET_BRIGHTNESS:
         brightness = msg->data[1];
         break;
      case CMD_SET_POSITION:
         encoder.position = msg->data[1];
         break;
      case CMD_SET_LED: 
         if (msg->len == 4) {
            set_led(i & CMD_DATA_MASK, msg->data[1], msg->data[2], msg->data[3]);
         } else {
            led_wheel(i & CMD_DATA_MASK, msg->data[1]); 
         }
         break;
   }
}

void clear_leds(void) {
   for (uint8_t i = 0; i < LED_COUNT; i++) {
      leds[i].r = 0;
      leds[i].g = 0;
      leds[i].b = 0;
   }
}

void set_led(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
   leds[i].r = (brightness * r) / 255;
   leds[i].g = (brightness * g) / 255;
   leds[i].b = (brightness * b) / 255;
}

void update_leds(void) {
   if (encoder.position < 0) {
      encoder.position = 0;
   }
   uint8_t ticks = (encoder.position/3);
   uint8_t tmp = ticks/2;
   for (uint8_t i = 0; i < LED_COUNT; i++) {
      if (ticks%2 && i==tmp+1) {
         set_led(i, 75, 50, 0);
      } else if (i <= tmp) {
         set_led(i, 0, 200, 0);
      } else {
         set_led(i, 0, 0, 0);
      }
   }
}

inline uint8_t animate(uint8_t curr, uint8_t target) {
   int16_t diff = (target+(curr*3))/4;
   return diff;
}

void update_leds_current(void) {
   for (uint8_t i = 0; i <LED_COUNT; i++) {
      leds_current[i].r = animate(leds_current[i].r, leds[i].r);
      leds_current[i].g = animate(leds_current[i].g, leds[i].g);
      leds_current[i].b = animate(leds_current[i].b, leds[i].b);
   }
}

int main (void) {
   CCP = 0xD8;
   CLKPSR = 0;

   twi_init(0x42);
   encoder_init();

   sei();
   while(1) {
      if (!manual_mode) {
         update_leds();
      }
      update_leds_current();
      ws2812_setleds(leds_current, LED_COUNT);
      _delay_ms(16);
   }
   return 0;
}
