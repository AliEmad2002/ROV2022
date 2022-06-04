/*
 * TIMER_interface.h
 *
 * Created: 13/03/2022 05:07:00 م
 *  Author: Ali Emad
 */ 

/************************************************************************/
/*                             NOTES :                                  */
/* timer0 and timer1 share the same prescaler.							*/

/************************************************************************/

#ifndef TIMER_INTERFACE_H_
#define TIMER_INTERFACE_H_

/************************************************************************/
/*                          General SYNC :                              */
/************************************************************************/
void TIM_voidHaltAllTimers (void) ;
void TIM_voidRunTimers (void) ;


/************************************************************************/
/*                             TIMER 0 :                                */
/************************************************************************/

enum TIM0And1ClockSource{TIM0And1_CS_STOP,TIM0And1_CS_DIV_1,TIM0And1_CS_DIV_8,TIM0And1_CS_DIV_64,TIM0And1_CS_DIV_256,TIM0And1_CS_DIV_1024,TIM0And1_CS_T0_FALLING,TIM0And1_CS_T0_RISING};

enum TIM0Mode{TIM0_MODE_NORMAL, TIM0_MODE_PCPWM_Top0xFF, TIM0_MODE_CTC, TIM0_MODE_FPWM_Top0xFF, TIM0_MODE_PCPWM_TopOCR=5, TIM0_MODE_FPWM_TopOCR=7};

enum TIM0CTC{TIM0_CTC_DIS, TIM0_CTC_TOG, TIM0_CTC_CLR, TIM0_CTC_SET};

enum TIM0PCPWM{TIM0_PCPWM_DIS, TIM0_PCPWM_TOG, TIM0_PCPWM_NON_INVERT, TIM0_PCPWM_INVERT};
                            // ^ only when mode is TIM0_MODE_PCPWM_TopOCR, otherwise it gives "Normal Port Operation, OC0x Disconnected"
enum TIM0FPWM{TIM0_FPWM_DIS, TIM0_FPWM_TOG, TIM0_FPWM_NON_INVERT, TIM0_FPWM_INVERT};
                          // ^ only when mode is TIM0_MODE_FPWM_TopOCR, otherwise it gives "Normal Port Operation, OC0x Disconnected"

enum TIM0OutputChannel {TIM0ChannelA, TIM0ChannelB} ;

enum TIM0Interrupt{TIM0InterruptOVF, TIM0InterruptCompA, TIM0InterruptCompB};

void MTIM0_voidInit(enum TIM0And1ClockSource clockSource, enum TIM0Mode mode, u8 channelAMode, u8 channelBMode, u8 preload) ;

u8 MTIM0_u8GetCounter(void);

void MTIM0_voidSetCounter(u8 counterValue);

void MTIM0_voidSetDutyCycle(u8 dutyCycle, enum TIM0OutputChannel channel);//dutyCycle will be a number between 0=>255 representing a duty cycle 0=>100%

void MTIM0_voidSetFrequency(u16 frequency, enum TIM0OutputChannel channel);//Sets CTC Frequency
// note that max freq = F_CPU / 512
// and prescaler matters !
// because : OCRx_register = F_CPU / 2*freq*N - 1

void MTIM0_voidDelayTicks(u64 ticks);

void MTIM0_voidEnableInterrupt(enum TIM0Interrupt mode);

void MTIM0_voidDisableInterrupt(enum TIM0Interrupt mode);

void MTIM0_voidSetCallBack(enum TIM0Interrupt mode, void(*CallBack)(void));

////////////////////////////////////////////////////THANKS TO OUR GOD ALLAH////////////////////////////////////////////////////////

/************************************************************************/
/*                             TIMER 1 :                                */
/************************************************************************/

// enum TIM0And1ClockSource is previously declared in TIMER0 stuff.
enum TIM1Mode{TIM1_MODE_NORMAL, TIM1_MODE_PCPWM_8bit_Top0x00FF, TIM1_MODE_PCPWM_9bit_Top0x01FF, TIM1_MODE_PCPWM_10bit_Top0x03FF, TIM1_MODE_CTC_TopOCR1A, TIM1_MODE_FPWM_8bit_Top0x00FF, TIM1_MODE_FPWM_9bit_Top0x01FF , TIM1_MODE_FPWM_10bit_Top0x03FF, TIM1_MODE_PFCPWM_TopICR1, TIM1_MODE_PFCPWM_TopOCR1A, TIM1_MODE_PCPWM_TopICR1, TIM1_MODE_PCPWM_TopOCR1A, TIM1_MODE_CTC_TopICR1, TIM1_MODE_FPWM_TopICR1=14, TIM1_MODE_FPWM_TopOCR1A} ;

enum TIM1CTC{TIM1_CTC_DIS, TIM1_CTC_TOG, TIM1_CTC_CLR, TIM1_CTC_SET};

enum TIM1PCPWM{TIM1_PCPWM_DIS, TIM1_PCPWM_TOG, TIM1_PCPWM_NON_INVERT, TIM1_PCPWM_INVERT};

enum TIM1FPWM{TIM1_FPWM_DIS, TIM1_FPWM_TOG, TIM1_FPWM_NON_INVERT, TIM1_FPWM_INVERT};

enum TIM1OutputChannel {TIM1ChannelA, TIM1ChannelB} ;

enum TIM1Interrupt{TIM1InterruptOVF, TIM1InterruptCompA, TIM1InterruptCompB, TIM1InterruptCapture};
	
enum TIM1CaptureInterrupt {TIM1_Capture_FallingEdge, TIM1_Capture_RisingEdge};
	
enum TIM1InputPulseType{TIM1_PULSE_LOW,TIM1_PULSE_HIGH};

void MTIM1_voidInit(enum TIM0And1ClockSource clockSource, enum TIM1Mode mode, u8 channelAMode, u8 channelBMode, u16 preload);

u16 MTIM1_u16GetCounter(void);

void MTIM1_voidSetCounter(u16 counterValue);

void MTIM1_voidSetDutyCycle(u16 dutyCycle, enum TIM1OutputChannel channel);

void MTIM1_voidSet8BitDutyCycle(u8 dutyCycle, enum TIM1OutputChannel channel);

void MTIM1_voidSetFrequency(u32 frequency, enum TIM1OutputChannel channel);//Sets CTC Frequency

void MTIM1_voidDelay_ticks(u64 ticks);

void MTIM1_voidSet_Capture_Edge (enum TIM1CaptureInterrupt captureEdge);

void MTIM1_voidSet_Noise_Canceller(u8 noise_canceller_state);

void MTIM1_voidEnableInterrupt(enum TIM1Interrupt mode);

void MTIM1_voidDisableInterrupt(enum TIM1Interrupt mode);

void MTIM1_voidSetCallBack(enum TIM1Interrupt mode, void* callBack);

volatile u64* MTIM1_u32ArrGetPulseDurationsuS(void);	// returns an array of: [highTime, lowTime]

f32 MTIM1_f32GetSignalFrequency(void);

void MTIM1_voidStartTickMeasure(void);

u32  MTIM1_u32GetElapsedTicks(void);

u16 MTIM1_u16GetDivision(void);

u16* MTIM0And1_u16ArrGetFirstLargerPrescaler(u16 prescalerTheo) ;

u32 MTIM1TOV1TCNT1 ;

////////////////////////////////////////////////////THANKS TO OUR GOD ALLAH////////////////////////////////////////////////////////

/************************************************************************/
/*                             TIMER 2 :                                */
/************************************************************************/
enum TIM2ClockSource{TIM2_CS_STOP,TIM2_CS_DIV_1,TIM2_CS_DIV_8, TIM2_CS_DIV_32,TIM2_CS_DIV_64, TIM2_CS_DIV_128,TIM2_CS_DIV_256,TIM2_CS_DIV_1024};

enum TIM2Mode{TIM2_MODE_NORMAL, TIM2_MODE_PCPWM_Top0xFF, TIM2_MODE_CTC, TIM2_MODE_FPWM_Top0xFF, TIM2_MODE_PCPWM_TopOCR=5, TIM2_MODE_FPWM_TopOCR=7};

enum TIM2CTC{TIM2_CTC_DIS,TIM2_CTC_TOG,TIM2_CTC_CLR,TIM2_CTC_SET};

enum TIM2PCPWM{TIM2_PCPWM_DIS, TIM2_PCPWM_TOG, TIM2_PCPWM_NON_INVERT, TIM2_PCPWM_INVERT};

enum TIM2FPWM{TIM2_FPWM_DIS, TIM2_FPWM_TOG, TIM2_FPWM_NON_INVERT, TIM2_FPWM_INVERT};

enum TIM2OutputChannel {TIM2ChannelA, TIM2ChannelB} ;

enum TIM2Interrupt{TIM2InterruptOVF, TIM2InterruptCompA, TIM2InterruptCompB};

void MTIM2_voidInit(enum TIM2ClockSource clockSource, enum TIM2Mode mode, u8 channelAMode, u8 channelBMode, u8 preload) ;

u8 MTIM2_u8GetCounter(void);

void MTIM2_voidSetCounter(u8 counterValue);

void MTIM2_voidSetDutyCycle(u8 dutyCycle, enum TIM2OutputChannel channel);//dutyCycle will be a number between 0=>255 representing a duty cycle 0=>100%

void MTIM2_voidSetFrequency(u16 frequency, enum TIM2OutputChannel channel);//Sets CTC Frequency
// note that max freq = F_CPU / 512
// and prescaler matters !
// because : OCRx_register = F_CPU / 2*freq*N - 1

void MTIM2_voidDelayTicks(u64 ticks);

void MTIM2_voidEnableInterrupt(enum TIM2Interrupt mode);

void MTIM2_voidDisableInterrupt(enum TIM2Interrupt mode);

void MTIM2_voidSetCallBack(enum TIM2Interrupt mode, void(*CallBack)(void));

u16* MTIM2_u16ArrGetFirstLargerPrescaler(u16 prescalerTheo) ;

#endif /* TIMER_INTERFACE_H_ */






