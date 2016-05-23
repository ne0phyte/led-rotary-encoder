#ifndef TWI_H_
#define TWI_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define TWI_SCL_PORT PORTA
#define TWI_SCL_DDR DDRA
#define TWI_SCL_PIN PA7

#define TWI_MSG_SIZE 4


#define TWI_MSG1(msg, a)   do{ \
   (msg)->data[0] = (a); \
   (msg)->len = 1; \
}while(0)

#define TWI_MSG2(msg, a, b)   do{ \
   (msg)->data[0] = (a); \
   (msg)->data[1] = (b); \
   (msg)->len = 2; \
}while(0)

#define TWI_MSG3(msg, a, b, c)   do{ \
   (msg)->data[0] = (a); \
   (msg)->data[1] = (b); \
   (msg)->data[2] = (c); \
   (msg)->len = 3; \
}while(0)

#define TWI_MSG4(msg, a, b, c, d)   do{ \
   (msg)->data[0] = (a); \
   (msg)->data[1] = (b); \
   (msg)->data[2] = (c); \
   (msg)->data[3] = (d); \
   (msg)->len = 4; \
}while(0)

typedef struct TWIMsg {
   uint8_t len;
   uint8_t data[TWI_MSG_SIZE];
} TWIMsg;

void twi_init(uint8_t address);
extern void twi_on_request(TWIMsg *msg); 
extern void twi_on_receive(TWIMsg *msg); 

#endif
