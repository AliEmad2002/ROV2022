#include "STD_TYPES.h"
#include "bitMath.h"

#include "CLOCK_interface.h"
#include "CLOCK_config.h"



void MCLOCK_voidDelayMS(u32 ms){
	register u32 Ticks = (CLOCK_SYSTEM_FREQUENCY/10000)*ms ; // Register variables tell the compiler to store the variable in CPU register instead of memory
	// 10000 is an exp. number, do better !
	while(Ticks--)
		__asm__ volatile("NOP"); // assembly instruction that does nothing.
}

u32 MCLOCK_u32GetSystemClock(void){
	return CLOCK_SYSTEM_FREQUENCY;
}
