/*
 * ESC_interface.h
 *
 * Created: 14/03/2022 09:45:28 م
 *  Author: Ali Emad
 */ 


#ifndef ESC_INTERFACE_H_
#define ESC_INTERFACE_H_

enum ESCTimers {ESC_Timer0, ESC_Timer1, ESC_Timer2} ;
	
enum ESCChannel {ESC_ChannelA, ESC_ChannelB} ;
	
enum ESCDir {ESC_DirForward, ESC_DirBackward} ;

typedef struct{
	enum ESCTimers timer ;		// the timer used to generate signal
	enum ESCChannel channel ;	// channel on that timer
	u32 minDutyCycle ;			// in us
	u32 stpDutyCycle ;			// in us
	u32 maxDutyCycle ;			// in us
	
	u8 minOCR ;					// 8-bit OCR register
	u8 stpOCR ;					// 8-bit OCR register
	u8 maxOCR ;					// 8-bit OCR register
	
	s16 currentSpeeed ; // negative for reverse, positive for forward.
	s16 targetSpeed ;
		
	//f32 T ;						// inverse of the generated FPWM (in us)
	
	void (*changeDutyCycleCallback)(u8 dutyCycle, u8 channel);	// timer function that changes OCR value (from timer driver)
}ESC_t;

void HESC_voidInit(ESC_t* ESC, enum ESCTimers timer, enum ESCChannel channel, u16 minDutyCycle, u16 stpDutyCycle, u16 maxDutyCycle, u16 frequency);

void HESC_voidChangeDirAndSpeed(ESC_t* ESC, enum ESCDir dir, u8 speed); // speed is a number between 0 and 100

void HESC_voidChangeSpeed(ESC_t* ESC, s16 speed); // speed is a number between -100 and 100

u8 HESC_u8OCR(ESC_t* ESC, enum ESCDir dir, u8 speed) ;

void HESC_voidChangeOCR(ESC_t* ESC, u8 OCR) ;


#endif /* ESC_INTERFACE_H_ */