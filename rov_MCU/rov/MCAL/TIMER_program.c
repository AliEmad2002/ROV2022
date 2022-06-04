/*
 * TIMER_program.c
 *
 * Created: 13/03/2022 05:07:23 م
 *  Author: Ali Emad
 */ 

#include "STD_TYPES.h"
#include "bitMath.h"
#include "INTERRUPT.h"
#include "CLOCK_interface.h"
#include "TIMER_interface.h"
#include "TIMER_private.h"
#include "MATH.h"
#include <stdlib.h>


/************************************************************************/
/*                             TIMER 0 :                                */
/************************************************************************/

void MTIM0_voidInit(enum TIM0And1ClockSource clockSource, enum TIM0Mode mode, u8 channelAMode, u8 channelBMode, u8 preload)
{
	// note that using outputting modes requires initializing the channel pin as an output !
	
	// set clock source :
	WRITE_BIT(TCCR0B_register, CS00, GET_BIT(clockSource, 0)) ;
	WRITE_BIT(TCCR0B_register, CS01, GET_BIT(clockSource, 1)) ;
	WRITE_BIT(TCCR0B_register, CS02, GET_BIT(clockSource, 2)) ;
	PTIM0And1Prescaler = PTIM0And1Prescalers[clockSource] ;
	
	// set mode :
	WRITE_BIT(TCCR0A_register, WGM00, GET_BIT(mode, 0)) ;
	WRITE_BIT(TCCR0A_register, WGM01, GET_BIT(mode, 1)) ;
	WRITE_BIT(TCCR0B_register, WGM02, GET_BIT(mode, 2)) ;
	PTIM0Mode = mode ;
	
	// set channel A mode :
	WRITE_BIT(TCCR0A_register, COM0A0, GET_BIT(channelAMode, 0)) ;
	WRITE_BIT(TCCR0A_register, COM0A1, GET_BIT(channelAMode, 1)) ;
	
	// set channel B mode :
	WRITE_BIT(TCCR0A_register, COM0B0, GET_BIT(channelBMode, 0)) ;
	WRITE_BIT(TCCR0A_register, COM0B1, GET_BIT(channelBMode, 1)) ;
	
	// preload TCNT0 register :
	TCNT0_register = preload ;
}

u8 MTIM0_u8GetCounter(void)
{
	return TCNT0_register ;
}

void MTIM0_voidSetCounter(u8 counterValue)
{
	TCNT0_register = counterValue ;
}

void MTIM0_voidSetDutyCycle(u8 dutyCycle, enum TIM0OutputChannel channel)
{
	switch(channel)
	{
		case TIM0ChannelA :
			OCR0A_register = dutyCycle ;
			break ;
			
		case TIM0ChannelB :
			OCR0B_register = dutyCycle ;
			break ;
	}
}

void MTIM0_voidSetFrequency(u16 frequency, enum TIM0OutputChannel channel)
{
	//Sets CTC Frequency
	// note that max freq = F_CPU / 512
	// and prescaler matters !
	// do later : a function that sets the best prescaler (like you've done in ATmega16/32 driver)
	// because : OCRx_register = F_CPU / 2*freq*N - 1
	
	u8 OCR = MCLOCK_u32GetSystemClock() / (2*frequency*PTIM0And1Prescaler) - 1 ;
	switch(channel)
	{
		case TIM0ChannelA :
		OCR0A_register = OCR ;
		break ;
		
		case TIM0ChannelB :
		OCR0B_register = OCR ;
		break ;
	}
}

void MTIM0_voidDelayTicks(u64 ticks)
{
	u8 last_TCNT0 = TCNT0_register ;
	while (ticks)
	{
		if (last_TCNT0 != TCNT0_register)
		{
			last_TCNT0 = TCNT0_register ;
			ticks-- ;
		}
	}
}

// Add the rest of delaying functions later, for now use the "MCLOCK_voidDelayMS()"

void MTIM0_voidEnableInterrupt(enum TIM0Interrupt mode)
{
	INTERRUPT_ENABLE_GLOBAL_INTERRUPT ;
	SET_BIT(TIMSK0_register, mode) ;
}

void MTIM0_voidDisableInterrupt(enum TIM0Interrupt mode)
{
	// doesn't need to "INTERRUPT_DISABLE_GLOBAL_INTERRUPT"
	CLR_BIT(TIMSK0_register, mode) ;
}

void MTIM0_voidSetCallBack(enum TIM0Interrupt mode, void(*CallBack)(void))
{
	switch (mode)
	{
		case TIM0InterruptOVF :
			PTIM0_callback_OVF = CallBack ;
			break ;
			
		case TIM0InterruptCompA :
			PTIM0_callback_COMPA = CallBack ;
			break ;
			
		case TIM0InterruptCompB :
			PTIM0_callback_COMPB = CallBack ;
			break ;
	}
}

// don't forget to assign callback functions to start inside VECT_ISR function :
void PTIM0_VECT_ISR_OVF(void)
{
	PTIM0_callback_OVF() ;
}

void PTIM0_VECT_ISR_COMPA(void)
{
	PTIM0_callback_COMPA() ;
}

void PTIM0_VECT_ISR_COMPB(void)
{
	PTIM0_callback_COMPB() ;
}

/************************************************************************/
/*                             TIMER 1 :                                */
/************************************************************************/
void MTIM1_voidInit(enum TIM0And1ClockSource clockSource, enum TIM1Mode mode, u8 channelAMode, u8 channelBMode, u16 preload)
{
	// note that using outputting modes requires initializing the channel pin as an output !
	
	// set clock source :
	WRITE_BIT(TCCR1B_register, CS10, GET_BIT(clockSource, 0)) ;
	WRITE_BIT(TCCR1B_register, CS11, GET_BIT(clockSource, 1)) ;
	WRITE_BIT(TCCR1B_register, CS12, GET_BIT(clockSource, 2)) ;
	PTIM0And1Prescaler = PTIM0And1Prescalers[clockSource] ;
	MTIM1TOV1TCNT1 = PTIM1TOV1TCNT1arr[mode] ;
	
	// set mode :
	WRITE_BIT(TCCR1A_register, WGM10, GET_BIT(mode, 0)) ;
	WRITE_BIT(TCCR1A_register, WGM11, GET_BIT(mode, 1)) ;
	WRITE_BIT(TCCR1B_register, WGM12, GET_BIT(mode, 2)) ;
	WRITE_BIT(TCCR1B_register, WGM13, GET_BIT(mode, 3)) ;
	PTIM1Mode = mode ;
	
	// set channel A mode :
	WRITE_BIT(TCCR1A_register, COM1A0, GET_BIT(channelAMode, 0)) ;
	WRITE_BIT(TCCR1A_register, COM1A1, GET_BIT(channelAMode, 1)) ;
	
	// set channel B mode :
	WRITE_BIT(TCCR1A_register, COM1B0, GET_BIT(channelBMode, 0)) ;
	WRITE_BIT(TCCR1A_register, COM1B1, GET_BIT(channelBMode, 1)) ;
	
	// refer to the datasheet and private .h file to recite the ATmega handling of 16-bit registers
	TCNT1_register = preload ;
	
	PTIM0And1OVFCount = 0 ;
	PTIM1StartTickMeasureFirstTime = 1 ;
	
	PTIMICPDuties = malloc(2*sizeof(u64)) ;
}

u16 MTIM1_u16GetCounter(void)
{
	return TCNT1_register;
}

void MTIM1_voidSetCounter(u16 counterValue)
{
	TCNT1_register = counterValue ;
}

void MTIM1_voidSetDutyCycle(u16 dutyCycle, enum TIM1OutputChannel channel)
{
	if(channel == TIM1ChannelA)
		OCR1A_register = dutyCycle;
	else if (channel == TIM1ChannelB)
		OCR1B_register = dutyCycle; //I can't find OCR1B as a top value in the datasheet?! or is it in place of these OCR1A's but for ChB ?... try it and find out ! - piss on my face if you do.
}

void MTIM1_voidSet8BitDutyCycle(u8 dutyCycle, enum TIM1OutputChannel channel)
{
	if(channel == TIM1ChannelA)
		OCR1A_register = (u16)dutyCycle ;
	else if (channel == TIM1ChannelB)
		OCR1B_register = (u16)dutyCycle ;
}

void MTIM1_voidSetFrequency(u32 frequency, enum TIM1OutputChannel channel)
{
	//Sets CTC Frequency
	u16 OCR = MCLOCK_u32GetSystemClock() / (2*frequency*PTIM0And1Prescaler) - 1;
	if(channel == TIM1ChannelA)
		OCR1A_register = OCR;
	else if(channel == TIM1ChannelB)
		OCR1B_register = OCR;
}

void MTIM1_voidDelay_ticks(u64 ticks)
{
	u16 last_TCNT1 = TCNT1_register ;
	while (ticks)
	{
		if (last_TCNT1 != TCNT1_register)
		{
			last_TCNT1 = TCNT1_register ;
			ticks-- ;
		}
	}
}

void MTIM1_voidSet_Capture_Edge (enum TIM1CaptureInterrupt captureEdge)
{
	WRITE_BIT(TCCR1B_register, ICES1, captureEdge) ;
}

void MTIM1_voidSet_Noise_Canceller(u8 noise_canceller_state)
{
	// noise_canceller_state = 1 ==> : noise canceller on
	// noise_canceller_state = 0 ==> : noise canceller off
	WRITE_BIT(TCCR1B_register, ICNC1, noise_canceller_state) ;
}

void MTIM1_voidEnableInterrupt(enum TIM1Interrupt mode)
{
	INTERRUPT_ENABLE_GLOBAL_INTERRUPT ;
	SET_BIT(TIMSK1_register, mode) ;
}

void MTIM1_voidDisableInterrupt(enum TIM1Interrupt mode)
{
	CLR_BIT(TIMSK1_register, mode) ;
}

void MTIM1_voidSetCallBack(enum TIM1Interrupt mode, void* callBack)
{
	if (mode == TIM1InterruptCapture)
		PTIM1_callback_CAPT = (void(*)(u16))callBack ;
	else if (mode == TIM1InterruptCompB)
		PTIM1_callback_COMPB = (void(*)(void))callBack;
	else if (mode == TIM1InterruptCompA)
		PTIM1_callback_COMPA = (void(*)(void))callBack;
	else // if (mode == TIM1InterruptOVF)
		PTIM1_callback_OVF = (void(*)(void))callBack;
}

volatile u64* MTIM1_u32ArrGetPulseDurationsuS(void)	// returns an array of: [highTime, lowTime]
{
	volatile register u64 t1 ;
	volatile register u64 t2 ;
	volatile register u64 t3 ;
	volatile register u64 OVFCount = 0 ;
	
	TCNT1_register = 0 ;
	
	// clearing ICF1, TOV1 flags :
	SET_BIT(TIFR1_register, ICF1) ;
	SET_BIT(TIFR1_register, TOV1) ;
	
	// wait for the rising edge to come :
	SET_BIT(TCCR1B_register, ICES1) ;
	while (!GET_BIT(TIFR1_register, ICF1))
		if (GET_BIT(TIFR1_register, TOV1)) // an OVF happened :
		{
			SET_BIT(TIFR1_register, TOV1) ; // clear TOV1 flag
			OVFCount++ ;
		}
	t1 = (u64)ICR1_register + OVFCount*65535ull ;
	// clearing ICF1 flag :
	SET_BIT(TIFR1_register, ICF1) ;
	
	// wait for the falling edge to come :
	CLR_BIT(TCCR1B_register, ICES1) ;
	while (!GET_BIT(TIFR1_register, ICF1))
		if (GET_BIT(TIFR1_register, TOV1)) // an OVF happened :
		{
			SET_BIT(TIFR1_register, TOV1) ; // clear TOV1 flag
			OVFCount++ ;
		}
	t2 = (u64)ICR1_register + OVFCount*65535ull ;
	// clearing ICF1 flag :
	SET_BIT(TIFR1_register, ICF1) ;
	
	// wait for another rising edge to come :
	SET_BIT(TCCR1B_register, ICES1) ;
	while (!GET_BIT(TIFR1_register, ICF1))
		if (GET_BIT(TIFR1_register, TOV1)) // an OVF happened :
		{
			SET_BIT(TIFR1_register, TOV1) ; // clear TOV1 flag
			OVFCount++ ;
		}
	t3 = (u64)ICR1_register + OVFCount*65535ull ;
	// clearing ICF1 flag :
	SET_BIT(TIFR1_register, ICF1) ;
	
	// now HIGH time and LOW time can be calculated :
	PTIMICPDuties[0] = 1000000ull * (t2 - t1) * (u64)PTIM0And1Prescaler / (u64)MCLOCK_u32GetSystemClock() ; // HIGH time
	PTIMICPDuties[1] = 1000000ull * (t3 - t2) * (u64)PTIM0And1Prescaler / (u64)MCLOCK_u32GetSystemClock() ; // LOW time
	return PTIMICPDuties ;
}

f32 MTIM1_f32GetSignalFrequency(void)
{
	// done it in ATmega32/16
	// later here...
	// not needed for the ROV.
	return 1.0 ;
}

void MTIM1_voidStartTickMeasure(void)
{
	//PTIM0And1OVFCount = 0 ;
	MTIM1_voidEnableInterrupt(TIM1InterruptOVF) ;
	if (PTIM1StartTickMeasureFirstTime == 1)
	{
		PTIM1FirstTCNT = TCNT1_register ;
		PTIM1StartTickMeasureFirstTime = 0 ;
	}
}

u32  MTIM1_u32GetElapsedTicks(void)
{
	return (u32)TCNT1_register + MTIM1TOV1TCNT1 * (u32)PTIM0And1OVFCount - (u32)PTIM1FirstTCNT ;
}

u16 MTIM1_u16GetDivision(void)
{
	return PTIM0And1Prescaler ;
}

void PTIM1_VECT_ISR_OVF (void)
{
	PTIM0And1OVFCount++;
	if (PTIM1_callback_OVF != 0) // i.e.: there was a callback set by user:
		PTIM1_callback_OVF() ;
}

void PTIM1_VECT_ISR_COMPB (void)
{
	PTIM1_callback_COMPB() ;
}

void PTIM1_VECT_ISR_COMPA (void)
{
	PTIM1_callback_COMPA() ;
}

void PTIM1_VECT_ISR_CAPT (void)
{
	PTIM1_callback_CAPT (ICR1_register) ;
}

u16* MTIM0And1_u16ArrGetFirstLargerPrescaler(u16 prescalerTheo)
{
	u16* arr = malloc(2*sizeof(u16)) ;
	arr[0] = MATH_u16GetFirstLargerIndex(prescalerTheo, PTIM0And1Prescalers, 8) ;
	arr[1] = PTIM0And1Prescalers[arr[0]] ;
	return arr ;
}


/************************************************************************/
/*                             TIMER 2 :                                */
/************************************************************************/
void MTIM2_voidInit(enum TIM2ClockSource clockSource, enum TIM2Mode mode, u8 channelAMode, u8 channelBMode, u8 preload)
{
	// note that using outputting modes requires initializing the channel pin as an output !
	
	// set clock source :
	WRITE_BIT(TCCR2B_register, CS20, GET_BIT(clockSource, 0)) ;
	WRITE_BIT(TCCR2B_register, CS21, GET_BIT(clockSource, 1)) ;
	WRITE_BIT(TCCR2B_register, CS22, GET_BIT(clockSource, 2)) ;
	PTIM2Prescaler = PTIM2Prescalers[clockSource] ;
	
	// set mode :
	WRITE_BIT(TCCR2A_register, WGM20, GET_BIT(mode, 0)) ;
	WRITE_BIT(TCCR2A_register, WGM21, GET_BIT(mode, 1)) ;
	WRITE_BIT(TCCR2B_register, WGM22, GET_BIT(mode, 2)) ;
	PTIM0Mode = mode ;
	
	// set channel A mode :
	WRITE_BIT(TCCR2A_register, COM2A0, GET_BIT(channelAMode, 0)) ;
	WRITE_BIT(TCCR2A_register, COM2A1, GET_BIT(channelAMode, 1)) ;
	
	// set channel B mode :
	WRITE_BIT(TCCR2A_register, COM2B0, GET_BIT(channelBMode, 0)) ;
	WRITE_BIT(TCCR2A_register, COM2B1, GET_BIT(channelBMode, 1)) ;
	
	// preload TCNT2 register :
	TCNT2_register = preload ;
}

u8 MTIM2_u8GetCounter(void)
{
	return TCNT2_register ;
}

void MTIM2_voidSetCounter(u8 counterValue)
{
	TCNT2_register = counterValue ;
}

void MTIM2_voidSetDutyCycle(u8 dutyCycle, enum TIM2OutputChannel channel)
{
	switch(channel)
	{
		case TIM2ChannelA :
			OCR2A_register = dutyCycle ;
			break ;
		
		case TIM2ChannelB :
			OCR2B_register = dutyCycle ;
			break ;
	}
}

void MTIM2_voidSetFrequency(u16 frequency, enum TIM2OutputChannel channel)
{
	//Sets CTC Frequency
	// note that max freq = F_CPU / 512
	// and prescaler matters !
	// do later : a function that sets the best prescaler (like you've done in ATmega16/32 driver)
	// because : OCRx_register = F_CPU / 2*freq*N - 1
	
	u8 OCR = MCLOCK_u32GetSystemClock() / (2*frequency*PTIM2Prescaler) - 1 ;
	switch(channel)
	{
		case TIM2ChannelA :
			OCR2A_register = OCR ;
			break ;
		
		case TIM2ChannelB :
			OCR0B_register = OCR ;
			break ;
	}
}

void MTIM2_voidDelayTicks(u64 ticks)
{
	u8 last_TCNT2 = TCNT2_register ;
	while (ticks)
	{
		if (last_TCNT2 != TCNT2_register)
		{
			last_TCNT2 = TCNT2_register ;
			ticks-- ;
		}
	}
}

// Add the rest of delaying functions later, for now use the "MCLOCK_voidDelayMS()"

void MTIM2_voidEnableInterrupt(enum TIM2Interrupt mode)
{
	INTERRUPT_ENABLE_GLOBAL_INTERRUPT ;
	SET_BIT(TIMSK2_register, mode) ;
}

void MTIM2_voidDisableInterrupt(enum TIM2Interrupt mode)
{
	// doesn't need to "INTERRUPT_DISABLE_GLOBAL_INTERRUPT"
	CLR_BIT(TIMSK2_register, mode) ;
}

void MTIM2_voidSetCallBack(enum TIM2Interrupt mode, void(*CallBack)(void))
{
	switch (mode)
	{
		case TIM2InterruptOVF :
			PTIM2_callback_OVF = CallBack ;
			break ;
		
		case TIM2InterruptCompA :
			PTIM2_callback_COMPA = CallBack ;
			break ;
		
		case TIM2InterruptCompB :
			PTIM2_callback_COMPB = CallBack ;
			break ;
	}
}

// don't forget to assign callback functions to start inside VECT_ISR function :
void PTIM2_VECT_ISR_OVF(void)
{
	PTIM2_callback_OVF() ;
}

void PTIM2_VECT_ISR_COMPA(void)
{
	PTIM2_callback_COMPA() ;
}

void PTIM2_VECT_ISR_COMPB(void)
{
	PTIM2_callback_COMPB() ;
}

u16* MTIM2_u16ArrGetFirstLargerPrescaler(u16 prescalerTheo)
{
	u16* arr = malloc(2*sizeof(u16)) ;
	arr[0] = MATH_u16GetFirstLargerIndex(prescalerTheo, PTIM2Prescalers, 8) ;
	arr[1] = PTIM2Prescalers[arr[0]] ;	
	return arr ;
}