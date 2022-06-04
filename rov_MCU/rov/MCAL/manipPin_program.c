/*
 * manipPin_program.c
 *
 * Created: 12/03/2022 09:22:19 م
 *  Author: Ali Emad
 */ 
#include <avr/io.h>
#include "STD_TYPES.h"
#include "bitMath.h"
#include "manipPin_private.h"
#include "manipPin_interface.h"


void MANIPPIN_voidInitPin(enum MANIPPIN_Port port, u8 pin, enum MANIPPIN_PinMode set)
{
	if (port == manipPin_Port_B)
	{
		if (set == INPUT)
			CLR_BIT(DDRB_register, pin) ;
		else
			SET_BIT(DDRB_register, pin) ;
	}
	else if (port == manipPin_Port_C)
	{
		if (set == INPUT)
			CLR_BIT(DDRC_register, pin) ;
		else
			SET_BIT(DDRC_register, pin) ;
	}
	else // if (port == manipPin_Port_D)
	{
		if (set == INPUT)
			CLR_BIT(DDRD_register, pin) ;
		else
			SET_BIT(DDRD_register, pin) ;
	}
}

void MANIPPIN_voidInitPin2(u8 pin, enum MANIPPIN_PinMode set) // same as previous function, but less inputs.
{
	//finding DDRn : by analyzing char pin MSN :
	u8 n = pin/0x0F;
	if(n*0x0F > pin-0x0A)
		n--;
	volatile u8* DDRnPtr = &DDRB_register + 3 * (n - 0xB) ;
	u8 pinNumber = pin - 0x0F * n - n;
	if (set == OUTPUT)
		*DDRnPtr |= 1<<pinNumber;
	else if (set == INPUT)
		*DDRnPtr &= ~(1<<pinNumber);
}

u8 MANIPPIN_u8DigitalRead (enum MANIPPIN_Port port, u8 pin)
{
	if (port == manipPin_Port_B)
		return GET_BIT(PINB_register, pin) ;
	else if (port == manipPin_Port_C)
		return GET_BIT(PINC_register, pin) ;
	else // if (port == manipPin_Port_D)
		return GET_BIT(PIND_register, pin) ;
}

u8 MANIPPIN_u8DigitalRead2 (u8 pin) // same as previous function, but less inputs.
{
	//finding PINn : by analyzing char pin MSN :
	u8 n = pin/0x0F;
	if(n*0x0F > pin-0x0A)
		n--;
	volatile u8* pinPtr = &PINB_register + 3 * (n - 0xB) ;
	u8 pinNumber = pin - 0x0F * n - n;
	return *pinPtr & (1<<pinNumber);
}

void MANIPPIN_voidSetPin (enum MANIPPIN_Port port, u8 pin, enum MANIPPIN_PinState set)
{
	if (port == manipPin_Port_B)
	{
		if (set == LOW)
			CLR_BIT(PORTB_register, pin) ;
		else if (set == HIGH)
			SET_BIT(PORTB_register, pin) ;
		else // if (set == TOGGLE)
			TOG_BIT(PORTB_register, pin) ;
	}
	else if (port == manipPin_Port_C)
	{
		if (set == LOW)
			CLR_BIT(PORTC_register, pin) ;
		else if (set == HIGH)
			SET_BIT(PORTC_register, pin) ;
		else //if (set == TOGGLE)
			TOG_BIT(PORTC_register, pin) ;
	}
	else //if (port == manipPin_Port_D)
	{
		if (set == LOW)
			CLR_BIT(PORTD_register, pin) ;
		else if (set == HIGH)
			SET_BIT(PORTD_register, pin) ;
		else //if (set == TOGGLE)
			TOG_BIT(PORTD_register, pin) ;
	}
}

void MANIPPIN_voidSetPin2 (u8 pin, enum MANIPPIN_PinState set) // same as previous function, but less inputs.
{
	//finding PORTn : by analyzing char pin MSN :
	u8 n = pin/0x0F;
	if(n*0x0F > pin-0x0A)
		n--;
	volatile u8* portPtr = &PORTB_register + 3 * (n - 0xB) ;
	u8 pinNumber = pin - 0x0F * n - n;
	if (set == LOW)
		*portPtr &= ~(1<<pinNumber);
	else if(set == HIGH)
		*portPtr |= 1<<pinNumber;
	else if(set == TOGGLE)
		*portPtr ^= 1<<pinNumber;
}