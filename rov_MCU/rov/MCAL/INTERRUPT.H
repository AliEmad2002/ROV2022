/*
 * INTERRUPT.h
 *
 * Created: 13/03/2022 07:29:42 م
 *  Author: Ali Emad
 */ 


#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define INTERRUPT_REG_SREG				(*(volatile u8*)0x5F)	// same in ATmega: 16/32/328

#define I_BIT 7

#define INTERRUPT_ENABLE_GLOBAL_INTERRUPT				(SET_BIT(INTERRUPT_REG_SREG, I_BIT))
// referring to any interrupt doc. in the datasheet, you will always find something like :
// "When the TOIE0 bit is written to one, and the I-bit in the Status Register is set,
//  the Timer/Counter0 Overflow interrupt is enabled."
#define INTERRUPT_DISABLE_GLOBAL_INTERRUPT				(CLR_BIT(INTERRUPT_REG_SREG, I_BIT))



#endif /* INTERRUPT_H_ */