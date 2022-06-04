/*
 * TIMER_private.h
 *
 * Created: 13/03/2022 05:07:12 م
 *  Author: Ali Emad
 */ 


#ifndef TIMER_PRIVATE_H_
#define TIMER_PRIVATE_H_

/************************************************************************/
/*                             TIMER 0 :                                */
/************************************************************************/
// registers :
#define TCCR0A_register ( *(volatile u8*) 0x44 )
#define TCCR0B_register ( *(volatile u8*) 0x45 )
#define TCNT0_register ( *(volatile u8*) 0x46 )
#define OCR0A_register ( *(volatile u8*) 0x47 )
#define OCR0B_register ( *(volatile u8*) 0x48 )
#define TIMSK0_register ( *(volatile u8*) 0x8E )
#define TIFR0_register ( *(volatile u8*) 0x35 )


#define GTCCR_register ( *(volatile u8*) 0x43 )


// bits :
// of TCCR0A :
#define WGM00 0
#define WGM01 1
#define COM0B0 4
#define COM0B1 5
#define COM0A0 6
#define COM0A1 7

// of TCCR0B :
#define CS00 0
#define CS01 1
#define CS02 2
#define WGM02 3
#define FOC0B 6
#define FOC0A 7

// of TIMSK0 :
#define TOIE0 0
#define OCIE0A 1
#define OCIE0B 2

// of TIFR0 :
#define TOV0 0
#define OCF0A 1
#define OCF0B 2


// vectors :
#define PTIM0_VECT_ISR_OVF __vector_16 // remember : VectorNo. - 1
#define PTIM0_VECT_ISR_COMPB __vector_15
#define PTIM0_VECT_ISR_COMPA __vector_14

// YOU NEED TO UNDERSTAND WTF IS THIS :
void PTIM0_VECT_ISR_OVF(void) __attribute__((signal, used));
void PTIM0_VECT_ISR_COMPB(void) __attribute__((signal, used));
void PTIM0_VECT_ISR_COMPA(void) __attribute__((signal, used));

// pointers to ISR callback functions :
void (*PTIM0_callback_OVF)(void);
void (*PTIM0_callback_COMPB)(void);
void (*PTIM0_callback_COMPA)(void);

// private values :
#define PTIM0And1Prescalers (u16[]){0, 1, 8, 64, 256, 1024, 0, 0} // last two are foo's (external colck!)
u16 PTIM0And1Prescaler ;
u16 PTIM0And1OVFCount ;
u8 PTIM0Mode ;


/************************************************************************/
/*                             TIMER 1 :                                */
/************************************************************************/
// registers and their bits :
#define TIFR1_register ( *(volatile u8*) 0x36 )
#define TOV1 0
#define OCF1A 1
#define OCF1B 2
#define ICF1 5

#define TIMSK1_register ( *(volatile u8*) 0x6F )
#define TOIE1 0
#define OCIE1A 1
#define OCIE1B 2
#define ICIE1 5

#define TCCR1A_register ( *(volatile u8*) 0x80 )
#define WGM10 0
#define WGM11 1
#define COM1B0 4
#define COM1B1 5
#define COM1A0 6
#define COM1A1 7

#define TCCR1B_register ( *(volatile u8*) 0x81 )
#define CS10 0
#define CS11 1
#define CS12 2
#define WGM12 3
#define WGM13 4
#define ICES1 6
#define ICNC1 7

#define TCCR1C_register ( *(volatile u8*) 0x82 )
#define FOC1B 6
#define FOC1A 7

#define TCNT1_register ( *(volatile u16*) 0x84 ) // both low and high bytes are accessed via the lower one first !
//#define TCNT1L_register ( *(volatile u8*) 0x84 )
//#define TCNT1H_register ( *(volatile u8*) 0x85 )

#define ICR1_register ( *(volatile u16*) 0x86 ) // both low and high bytes are accessed via the lower one first !
//#define ICR1L_register ( *(volatile u8*) 0x86 )
//#define ICR1H_register ( *(volatile u8*) 0x87 )

#define OCR1A_register ( *(volatile u16*) 0x88 ) // both low and high bytes are accessed via the lower one first !
//#define OCR1AL_register ( *(volatile u8*) 0x88 )
//#define OCR1AH_register ( *(volatile u8*) 0x89 )

#define OCR1B_register ( *(volatile u16*) 0x8A ) // both low and high bytes are accessed via the lower one first !
//#define OCR1BL_register ( *(volatile u8*) 0x8A )
//#define OCR1BH_register ( *(volatile u8*) 0x8B )



// vectors :
#define PTIM1_VECT_ISR_OVF __vector_13 // remember : VectorNo. - 1
#define PTIM1_VECT_ISR_COMPB __vector_12
#define PTIM1_VECT_ISR_COMPA __vector_11
#define PTIM1_VECT_ISR_CAPT __vector_10

// YOU NEED TO UNDERSTAND WTF IS THIS :
void PTIM1_VECT_ISR_OVF(void) __attribute__((signal, used));
void PTIM1_VECT_ISR_COMPB(void) __attribute__((signal, used));
void PTIM1_VECT_ISR_COMPA(void) __attribute__((signal, used));
void PTIM1_VECT_ISR_CAPT(void) __attribute__((signal, used));

// pointers to ISR callback functions :
void (*PTIM1_callback_OVF)(void);
void (*PTIM1_callback_COMPB)(void);
void (*PTIM1_callback_COMPA)(void);
void (*PTIM1_callback_CAPT)(u16);

// private values :
// PTIM0And1Prescalers and PTIM0And1Prescaler are previously declared in TIMER0 stuff.
u8 PTIM1Mode ;
volatile u64* PTIMICPDuties ;
#define PTIM1TOV1TCNT1arr (u32[]){0xFFFF, 0x00FF+1, 0x01FF+1, 0x03FF+1, 0xFFFF, 0X00FF, 0X01FF, 0X03FF, ICR1_register+1, OCR1A_register+1, ICR1_register+1, OCR1A_register+1, 0XFFFF} // the value of TCNT1 at which TOV1 will be set (used for tick measure function)
u32 PTIM1FirstTCNT ;
u8 PTIM1StartTickMeasureFirstTime ;

/************************************************************************/
/*                             TIMER 2 :                                */
/************************************************************************/
// registers :
#define TCCR2A_register ( *(volatile u8*) 0xB0 )
#define TCCR2B_register ( *(volatile u8*) 0xB1 )
#define TCNT2_register ( *(volatile u8*) 0xB2 )
#define OCR2A_register ( *(volatile u8*) 0xB3 )
#define OCR2B_register ( *(volatile u8*) 0xB4 )
#define TIMSK2_register ( *(volatile u8*) 0x70 )
#define TIFR2_register ( *(volatile u8*) 0x37 )


// bits :
// of TCCR0A :
#define WGM20 0
#define WGM21 1
#define COM2B0 4
#define COM2B1 5
#define COM2A0 6
#define COM2A1 7

// of TCCR0B :
#define CS20 0
#define CS21 1
#define CS22 2
#define WGM22 3
#define FOC2B 6
#define FOC2A 7

// of TIMSK0 :
#define TOIE2 0
#define OCIE2A 1
#define OCIE2B 2

// of TIFR0 :
#define TOV2 0
#define OCF2A 1
#define OCF2B 2


// vectors :
#define PTIM2_VECT_ISR_OVF __vector_9 // remember : VectorNo. - 1
#define PTIM2_VECT_ISR_COMPB __vector_8
#define PTIM2_VECT_ISR_COMPA __vector_7

// YOU NEED TO UNDERSTAND WTF IS THIS :
void PTIM2_VECT_ISR_OVF(void) __attribute__((signal, used));
void PTIM2_VECT_ISR_COMPB(void) __attribute__((signal, used));
void PTIM2_VECT_ISR_COMPA(void) __attribute__((signal, used));

// pointers to ISR callback functions :
void (*PTIM2_callback_OVF)(void);
void (*PTIM2_callback_COMPB)(void);
void (*PTIM2_callback_COMPA)(void);

// private values :
#define PTIM2Prescalers (u16[]){0, 1, 8, 32, 64, 128, 256, 1024}
u16 PTIM2Prescaler ;
u8 PTIM2Mode ;

#endif /* TIMER_PRIVATE_H_ */