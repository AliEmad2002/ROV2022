/*
 * ESC_program.c
 *
 * Created: 14/03/2022 09:45:15 م
 *  Author: Ali Emad
 */ 

#include "STD_TYPES.h"
#include "bitMath.h"
#include "manipPin_interface.h"
#include "CLOCK_interface.h"
#include "TIMER_interface.h"
#include "ESC_interface.h"
#include "MATH.h"
#include <stdlib.h>

void HESC_voidInit(ESC_t* ESC, enum ESCTimers timer, enum ESCChannel channel, u16 minDutyCycle, u16 stpDutyCycle, u16 maxDutyCycle, u16 frequency)
{
	ESC->timer = timer ;
	ESC->channel = channel ;
	ESC->minDutyCycle = minDutyCycle ;
	ESC->stpDutyCycle = stpDutyCycle ;
	ESC->maxDutyCycle = maxDutyCycle ;
	
	ESC->currentSpeeed = 0 ;
	
	u16 prescaler = 0 ;
	
	// using 8-bit PWM :
	u16 prescalerTheo = MCLOCK_u32GetSystemClock() / (frequency * 256UL) ;
	
	if (timer == ESC_Timer0)
	{
		// set I/O pin as output :
		if (channel == ESC_ChannelA)
			MANIPPIN_voidInitPin(manipPin_Port_D, 6, OUTPUT) ;
		else
			MANIPPIN_voidInitPin(manipPin_Port_D, 5, OUTPUT) ;
			
		// set call back function of the ESC object to the change duty function of the timer :
		ESC->changeDutyCycleCallback = MTIM0_voidSetDutyCycle ;
		
		// round that theoretical prescaler to the first larger actual one :
		u16* arr = MTIM0And1_u16ArrGetFirstLargerPrescaler(prescalerTheo) ;
		u8 bestPrescalerIndex = arr[0] ;
		prescaler = arr[1] ;
		free(arr) ;
		
		// init that timer with the calculated prescaler, 8-bit FPWM, non-inverting mode :
		MTIM0_voidInit(bestPrescalerIndex, TIM0_MODE_FPWM_Top0xFF, TIM0_FPWM_NON_INVERT, TIM0_FPWM_NON_INVERT, 0) ;
	}
	
	else if (timer == ESC_Timer1)
	{
		// set I/O pin as output :
		if (channel == ESC_ChannelA)
			MANIPPIN_voidInitPin(manipPin_Port_B, 1, OUTPUT) ;
		else
			MANIPPIN_voidInitPin(manipPin_Port_B, 2, OUTPUT) ;
		
		// set call back function of the ESC object to the change duty function of the timer :
		ESC->changeDutyCycleCallback = MTIM1_voidSet8BitDutyCycle ;
		
		// round that theoretical prescaler to the nearest actual one :
		u16* arr = MTIM0And1_u16ArrGetFirstLargerPrescaler(prescalerTheo) ;
		u8 bestPrescalerIndex = arr[0] ;
		prescaler = arr[1] ;
		free(arr) ;
		
		// init that timer with the calculated prescaler, 8-bit FPWM, non-inverting mode :
		MTIM1_voidInit(bestPrescalerIndex, TIM1_MODE_FPWM_8bit_Top0x00FF, TIM1_FPWM_NON_INVERT, TIM1_FPWM_NON_INVERT, 0) ;
	}
	
	else // if (timer == ESC_Timer2)
	{	
		// set I/O pin as output :
		if (channel == ESC_ChannelA)
			MANIPPIN_voidInitPin(manipPin_Port_B, 3, OUTPUT) ;
		else
			MANIPPIN_voidInitPin(manipPin_Port_D, 3, OUTPUT) ;

		// set call back function of the ESC object to the change duty function of the timer :
		ESC->changeDutyCycleCallback = MTIM2_voidSetDutyCycle ;
		
		// round that theoretical prescaler to the nearest actual one :
		u16* arr = MTIM2_u16ArrGetFirstLargerPrescaler(prescalerTheo) ;
		u8 bestPrescalerIndex = 6 ;
		prescaler = 256 ;
		free(arr) ;
		
		// init that timer with the calculated prescaler, 8-bit FPWM, non-inverting mode :
		MTIM2_voidInit(bestPrescalerIndex, TIM2_MODE_FPWM_Top0xFF, TIM2_FPWM_NON_INVERT, TIM2_FPWM_NON_INVERT, 0) ;
		// تلصيمة صلحها بعديييين
	}
	
	// time of a complete cycle :
	u32 T = (u32)((u64)prescaler * 256000000ull / (u64)MCLOCK_u32GetSystemClock()) ;
	
	// parameters used in change speed function :
	ESC->minOCR = (u8)((u32)minDutyCycle * 255ul / T) ;
	ESC->stpOCR = (u8)((u32)stpDutyCycle * 255ul / T) ;
	ESC->maxOCR = (u8)((u32)maxDutyCycle * 255ul / T) ;
	
	// initially set on stop duty to arm the motor :
	ESC->changeDutyCycleCallback(ESC->stpOCR, ESC->channel) ;
	
	// wait for ESC to finish initialization :
	//MCLOCK_voidDelayMS(7000) ;
}

u8 HESC_u8OCR(ESC_t* ESC, enum ESCDir dir, u8 speed)
{
	u8 OCR ;
	
	// referring to the datasheet of the used module at https://bluerobotics.com/store/thrusters/speed-controllers/besc30-r3/ :
	
	if (dir == ESC_DirForward) // if dir is forward, map an OCR value between stpOCR and maxOCR :
		OCR = (u8)((u16)speed * (u16)(ESC->maxOCR - ESC->stpOCR) / 100u + (u16)ESC->stpOCR) ;
	
	else // if dir is backward, map an OCR between minOCR and stpOCR, but inverted :
		OCR = (u8)((u16)(100 - speed) * (u16)(ESC->stpOCR - ESC->minOCR) / 100u + (u16)ESC->minOCR) ;
	
	return OCR ;
}

void HESC_voidChangeOCR(ESC_t* ESC, u8 OCR)
{
	ESC->changeDutyCycleCallback(OCR, ESC->channel) ;
}

void HESC_voidChangeDirAndSpeed(ESC_t* ESC, enum ESCDir dir, u8 speed)
{
	 // speed is a number between 0 and 100
	 u8 OCR = HESC_u8OCR(ESC, dir, speed) ;
	 HESC_voidChangeOCR(ESC, OCR) ;
	 
	 if (dir == ESC_DirForward)
		ESC->currentSpeeed = (s16)speed ;
	else
		ESC->currentSpeeed = -1*((s16)speed) ;
}

void HESC_voidChangeSpeed(ESC_t* ESC, s16 speed) // speed is a number between -100 and 100
{
	enum ESCDir dir ;
	u8 speedMagnitude ;
	if (speed < 0)
	{
		dir = ESC_DirBackward ;
		speedMagnitude = (u8)(-speed) ;
	}
	else
	{
		dir = ESC_DirForward ;
		speedMagnitude = (u8)speed ;
	}
	HESC_voidChangeDirAndSpeed(ESC, dir, speedMagnitude) ;
}