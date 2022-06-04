/*
 * manipPin_private.h
 *
 * Created: 12/03/2022 09:53:24 م
 *  Author: Ali Emad
 */ 


#ifndef MANIPPIN_PRIVATE_H_
#define MANIPPIN_PRIVATE_H_

#define PORTD_register    ( *(volatile u8 *) 0x2B )
#define DDRD_register     ( *(volatile u8 *) 0x2A )
#define PIND_register     ( *(volatile u8 *) 0x29 )

#define PORTC_register    ( *(volatile u8 *) 0x28 )
#define DDRC_register     ( *(volatile u8 *) 0x27 )
#define PINC_register     ( *(volatile u8 *) 0x26 )

#define PORTB_register    ( *(volatile u8 *) 0x25 )
#define DDRB_register     ( *(volatile u8 *) 0x24 )
#define PINB_register     ( *(volatile u8 *) 0x23 )





#endif /* MANIPPIN_PRIVATE_H_ */