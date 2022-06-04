/*
 * LCD_interface.h
 *
 * Created: 24/03/2022 08:52:21 م
 *  Author: Ali Emad
 */ 


#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_

enum HLCD_mode{HLCD8BitsMode, HLCD4BitsMode} ;
	
enum HLCD_cursorMode{HLCDCursorOffBlinkOff=0x0C, HLCDCursorOffBlinkOn, HLCDCursorOnBlinkOff, HLCDCursorOnBlinkOn};

typedef struct 
{
	u8* dataPins ;
	u8 RSPin ;
	u8 RWPin ;
	u8 ENPin ;
	enum HLCD_mode mode ;
	
	
	
}LCD_t;


void HLCD_voidInit(LCD_t* LCD, u8* dataPins, u8 RSPin, u8 RWPin, u8 ENPin, enum HLCD_mode mode4Or8, enum HLCD_cursorMode cursorMode) ;

void HLCD_voidSetCursorMode(LCD_t* LCD, enum HLCD_cursorMode mode) ;

void HLCD_voidClearDisplay(LCD_t* LCD) ;

void HLCD_voidSendData(LCD_t* LCD, u8 data) ;

void HLCD_voidWriteBusAndLatch(LCD_t* LCD, u8 busVal) ;

void HLCD_voidLatch(LCD_t* LCD) ;

void HLCD_voidPrintString(LCD_t* LCD, char* str) ;

#endif /* LCD_INTERFACE_H_ */