 /*
 * LCD_program.c
 *
 * Created: 24/03/2022 08:52:07 م
 *  Author: Ali Emad
 */
 

#include "STD_TYPES.h"
#include "bitMath.h"
#include "manipPin_interface.h"
#include "CLOCK_interface.h"
#include "LCD_interface.h"
#include "LCD_private.h"


void HLCD_voidInit(LCD_t* LCD, u8* dataPins, u8 RSPin, u8 RWPin, u8 ENPin, enum HLCD_mode mode4Or8, enum HLCD_cursorMode cursorMode)
{
	// init pins as outputs:
	u8 dataPinsNumber = (mode4Or8 == HLCD8BitsMode) ? 8 : 4 ;
	for (u8 i=0; i<dataPinsNumber; i++)
		MANIPPIN_voidInitPin2(dataPins[i], OUTPUT) ;
		
	MANIPPIN_voidInitPin2(RSPin, OUTPUT) ;
	MANIPPIN_voidInitPin2(RWPin, OUTPUT) ;
	MANIPPIN_voidInitPin2(ENPin, OUTPUT) ;
	
	// store function arguments in LCD_t object :
	LCD->dataPins = dataPins ;
	LCD->RSPin = RSPin ;
	LCD->RWPin = RWPin ;
	LCD->ENPin = ENPin ;
	LCD->mode = mode4Or8 ;
	
	// give LCD-module some time to start up before communicating with it :
	MCLOCK_voidDelayMS(50) ;
	
	// 4-bit init :
	if (mode4Or8 == HLCD4BitsMode)
		HLCD_voidSendCMD(LCD, 0x02) ;
	
	// mode init command :
	HLCD_voidSendCMD(LCD, (mode4Or8 == HLCD8BitsMode) ? 0x38 : 0x28) ;
	
	// cursor mode :
	HLCD_voidSetCursorMode(LCD, cursorMode) ;
	HLCD_voidSendCMD(LCD, 0x06) ; // auto shifting cursor
	
	// clear display :
	HLCD_voidClearDisplay(LCD) ;
	
	// make courser go home :
	HLCD_voidSendCMD(LCD, 0x80) ;
	PLCD_u8CursorIndex = 0 ;
}

void HLCD_voidSetCursorMode(LCD_t* LCD, enum HLCD_cursorMode mode)
{
	HLCD_voidSendCMD(LCD, mode) ;
}

void HLCD_voidClearDisplay(LCD_t* LCD)
{
	HLCD_voidSendCMD(LCD, 0x01) ;
	// reset cursor's index variable :
	PLCD_u8CursorIndex = 0 ;
	// here execution time is more that other instructions :
	MCLOCK_voidDelayMS(1) ;
}

void HLCD_voidSendCMD(LCD_t* LCD, u8 cmd)
{
	// select the command register of the LCD-module :
	MANIPPIN_voidSetPin2(LCD->RSPin, LOW) ;
	
	// write cmd to the bus, and LCD-module latch it :
	HLCD_voidWriteBusAndLatch(LCD, cmd) ;
	
	// execution time :
	MCLOCK_voidDelayMS(3) ;
}

void HLCD_voidSendData(LCD_t* LCD, u8 data)
{
	// check if cursor needs to be shifted in row (LCD module won't do it on its own) :
	if (PLCD_u8CursorIndex == 32)
		HLCD_voidClearDisplay(LCD) ;
	if (PLCD_u8CursorIndex == 16)
		// force the cursor to the beginning of the 2nd line :
		HLCD_voidSendCMD(LCD, 0xC0) ;
	
	if (data != '\n')
	{
		PLCD_u8CursorIndex++ ;
	
		// select the data register of the LCD-module :
		MANIPPIN_voidSetPin2(LCD->RSPin, HIGH) ;
	
		// write data to the bus, and LCD-module latch it :
		HLCD_voidWriteBusAndLatch(LCD, data) ;
	}
	else
		// force the cursor to the beginning of the 2nd line :
		HLCD_voidSendCMD(LCD, 0xC0) ;
}

void HLCD_voidWriteBusAndLatch(LCD_t* LCD, u8 busVal)
{
	// select write mode :
	MANIPPIN_voidSetPin2(LCD->RWPin, LOW) ;
	
	// check width of the data bus (8 or 4 bits) :
	if (LCD->mode == HLCD8BitsMode)
	{
		// write the data at once :
		for (u8 i=0; i<8; i++)
			MANIPPIN_voidSetPin2(LCD->dataPins[i], GET_BIT(busVal, i)) ;
		HLCD_voidLatch(LCD) ;
	}
	
	else
	{
		// write the data in two times :
		// higher 4 bits first :
		for (u8 i=0; i<4; i++)
			MANIPPIN_voidSetPin2(LCD->dataPins[i], GET_BIT(busVal, (i+4))) ;
		HLCD_voidLatch(LCD) ;
		// lower 4 bits first :
		for (u8 i=0; i<4; i++)
			MANIPPIN_voidSetPin2(LCD->dataPins[i], GET_BIT(busVal, i)) ;
		HLCD_voidLatch(LCD) ;
	}
}

void HLCD_voidLatch(LCD_t* LCD)
{
	// make EN pin HIGH, then LOW, to make the LCD-module latch the data bus :
	MANIPPIN_voidSetPin2(LCD->ENPin, HIGH) ;
	MCLOCK_voidDelayMS(1) ;
	MANIPPIN_voidSetPin2(LCD->ENPin, LOW) ;
}

void HLCD_voidPrintString(LCD_t* LCD, char* str)
{
	for (u8 i=0; str[i]!='\0'; i++)
	{
		HLCD_voidSendData(LCD, str[i]) ;
	}
}