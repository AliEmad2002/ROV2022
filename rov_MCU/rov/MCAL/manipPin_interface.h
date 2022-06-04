/*
 * manipPin_interface.h
 *
 * Created: 12/03/2022 09:25:20 م
 *  Author: Ali Emad
 */ 


#ifndef MANIPPIN_INTERFACE_H_
#define MANIPPIN_INTERFACE_H_


#define B0 0xB0
#define B1 0xB1
#define B2 0xB2
#define B3 0xB3
#define B4 0xB4
#define B5 0xB5
#define B6 0xB6
#define B7 0xB7

#define C0 0xC0
#define C1 0xC1
#define C2 0xC2
#define C3 0xC3
#define C4 0xC4
#define C5 0xC5
#define C6 0xC6
#define C7 0xC7

#define D0 0xD0
#define D1 0xD1
#define D2 0xD2
#define D3 0xD3
#define D4 0xD4
#define D5 0xD5
#define D6 0xD6
#define D7 0xD7


enum MANIPPIN_Port {manipPin_Port_B, manipPin_Port_C, manipPin_Port_D};

enum MANIPPIN_PinMode {INPUT, OUTPUT};

enum MANIPPIN_PinState {LOW, HIGH, TOGGLE};
	
	
void MANIPPIN_voidInitPin(enum MANIPPIN_Port port, u8 pin, enum MANIPPIN_PinMode set);
void MANIPPIN_voidInitPin2(u8 pin, enum MANIPPIN_PinMode set); // same as previous function, but less inputs, more cycles.

void MANIPPIN_voidSetPin(enum MANIPPIN_Port port, u8 pin, enum MANIPPIN_PinState set);
void MANIPPIN_voidSetPin2(u8 pin, enum MANIPPIN_PinState set); // same as previous function, but less inputs, more cycles.

u8 MANIPPIN_u8DigitalRead (enum MANIPPIN_Port port, u8 pin);
u8 MANIPPIN_u8DigitalRead2 (u8 pin); // same as previous function, but less inputs, more cycles.

#endif /* MANIPPIN_INTERFACE_H_ */