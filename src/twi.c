#include "twi.h"

#define TWI_SCL_INIT (TWI_SCL_DDR |= _BV(TWI_SCL_PIN))
#define TWI_SCL_HIGH (TWI_SCL_PORT |= _BV(TWI_SCL_PIN))
#define TWI_SCL_LOW  (TWI_SCL_PORT &= ~(_BV(TWI_SCL_PIN)))

TWIMsg msg;
volatile uint8_t pos;

void twi_init(uint8_t address) {
   TWI_SCL_INIT;
   TWI_SCL_LOW;
   TWSA = (address << 1);
   TWSCRA = (_BV(TWSHE) | _BV(TWDIE) | _BV(TWASIE) | _BV(TWEN) | _BV(TWSIE));
}

ISR(TWI_SLAVE_vect)
{
   TWI_SCL_HIGH;

   uint8_t status = TWSSRA;
   if ( (status & (_BV(TWC) | _BV(TWBE))) ) {
      // error or collision
      // clear flags
      TWSSRA |= (_BV(TWASIF) | _BV(TWDIF) | _BV(TWBE));
      return;
   }
   uint8_t twdir = (status & _BV(TWDIR));
   if (status & _BV(TWASIF)) {
      if ((status & _BV(TWAS))) { // start
         pos = 0;
         msg.len = 0;
         if (twdir) { // master read
            twi_on_request(&msg);
         //} else { // master write
         //   msg.len = 0;
         }
      } else { // stop 
        if (!twdir) { // master write stop
          twi_on_receive(&msg);
        }
        TWSSRA = _BV(TWASIF); // clear interrupt
      }
   } else if (status & _BV(TWDIF)) {
      if (twdir) {
         if (pos >= msg.len) {
            TWSCRB = 0b0010; // wait for start
            return;
         }
         TWSD = msg.data[pos++];
      } else {
         if (msg.len >= TWI_MSG_SIZE) {
            TWSCRB = 0b0110; // Send NACK and wait for any START condition
            return;
         }
         msg.data[msg.len++] = TWSD;
      }
   }
   // ACK
   TWSCRB = 0b0011;
   TWI_SCL_LOW;
}


