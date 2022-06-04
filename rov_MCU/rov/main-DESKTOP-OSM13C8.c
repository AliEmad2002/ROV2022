/*
 * main.c
 *
 * Created: 4/3/2022 6:31:59 PM
 * Author: Mohamed Mahmoud Abdo
 * Edited: Ali Emad Ali
 */ 

/*
	0 up motor pin  PD6
	1 down motor pin  PD5
	
	2 left_forwad motor pin PB2
	3 left_backward motor pin PB1
	
	4 right_backward motor pin PB3
	5 right_forwad motor pin PD3
*/

// modes :
#define DEBUG_MODE 0
#define SAFE_SPEED_CHANGE_MODE 0

#include "STD_TYPES.h"
#include "bitMath.h"
#include "ESC_interface.h"
#include "CLOCK_interface.h"
#include "I2C_interface.h"
#include "manipPin_interface.h"
#include "PID.h"
#include "LCD_interface.h"
#include <stdlib.h>
	
#define SLAVE_ADRR 0x20

#define CHANGE_SPEED_DELAY 3 //ms

#define ALTITUDE_Kp 0.5f
#define ALTITUDE_Ki 0.5f
#define ALTITUDE_Kd 0.5f
#define ALTITUDE_REFRESH_RATE 0.03f // sec

#define ROLL_Kp 0.5f
#define ROLL_Ki 0.5f
#define ROLL_Kd 0.5f
#define ROLL_REFRESH_RATE 0.03f // sec

void Init_ESCs (ESC_t* ESCS) ;
void Forward (ESC_t* ESCS, u8 speed);
void Backward (ESC_t* ESCS, u8 speed);
void Right (ESC_t* ESCS, u8 speed);
void Left (ESC_t* ESCS, u8 speed);
void Up (ESC_t* ESCS, u8 speed);
void Down (ESC_t* ESCS, u8 speed);
void Yaw_r (ESC_t* ESCS, u8 speed);
void Yaw_l (ESC_t* ESCS, u8 speed);

int main(void)
{
	MCLOCK_voidDelayMS(6000) ; // wait for ESC's to get powered and stable.
	
	// init them :
	ESC_t* ESCS = (ESC_t*)malloc(6*sizeof(ESC_t)) ;
	Init_ESCs(ESCS) ;
	
	// init I2C :
	MI2C_voidSlaveInit(0x20, 0, 1) ;
	char* data ;
	char key ;
	u8 value ; // must be +ve, and of 3 digits !
	
	// init PID :
	PID_controller altitudePID ;
	PID_controller rollPID ;
	init_PID(&altitudePID, ALTITUDE_Kp, ALTITUDE_Ki, ALTITUDE_Kd, -100.0f, 100.0f, ALTITUDE_REFRESH_RATE, 0.001f) ;
	init_PID(&rollPID, ROLL_Kp, ROLL_Ki, ROLL_Kd, -100.0f, 100.0f, ROLL_REFRESH_RATE, 0.001f) ;
	f32 altitudeCurrent = 0 ; // cm
	f32 altitudeRequiered = 0 ; // cm
	f32 thetaRollCurrent = 0 ; // degree
	f32 thetaRollRequiered = 0 ; // degree
	
    while(1)
    {	
		if(MI2C_voidSlaveIsCalled()) //Rpi has sent new data :
		{
			data = MI2C_u8SlaveReadString();
			key = data[0];
			value = (u8)(data[1]-'0')*100 + (u8)(data[2]-'0')*10 + (u8)(data[3]-'0');
			
			if (key=='U')
				Up(ESCS, value);
			else if (key=='D')
				Down(ESCS, value);
			else if (key=='F')
				Forward(ESCS, value);
			else if (key=='B')
				Backward(ESCS, value);
			else if (key=='R')
				Right(ESCS, value);
			else if (key=='L')
				Left(ESCS, value);
			else if (key=='X')
				Yaw_r(ESCS, value);
			else if (key=='Y')
				Yaw_l(ESCS, value);
			else if (key=='Z') // user wants ROV to go to that altitude
				altitudeRequiered = (f32)value ;
			else if (key=='z') // sensor reading of current ROV's altitude
				altitudeCurrent = (f32)value ;
			else if (key=='Q') // user wants ROV to make that theta_roll
				thetaRollRequiered = (f32)value ;
			else if (key=='q') // sensor reading of current ROV's theta_roll
				thetaRollCurrent = (f32)value ;
			
		}
		
		if (isPID_time(&altitudePID))
		{
			f32 speed = PID_update(&altitudePID, altitudeRequiered, altitudeCurrent) ;
			//HESC_voidChangeSpeed(ESCS+0, (s8)speed) ;
		}
		
		if (isPID_time(&rollPID))
		{
			f32 speed = PID_update(&rollPID, thetaRollRequiered, thetaRollCurrent) ;
			//HESC_voidChangeSpeed(ESCS+1, (s8)speed) ;
		}
    }
}

void Init_ESCs (ESC_t* ESCS)
{
	HESC_voidInit(ESCS+0, ESC_Timer0, ESC_ChannelA, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+1, ESC_Timer0, ESC_ChannelB, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+2, ESC_Timer1, ESC_ChannelA, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+3, ESC_Timer1, ESC_ChannelB, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+4, ESC_Timer2, ESC_ChannelA, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+5, ESC_Timer2, ESC_ChannelB, 1100, 1500, 1900, 400) ;
	
	MCLOCK_voidDelayMS(7000) ; // wait for ESC's to get init.
}

void Forward (ESC_t* ESCS, u8 speed)
{
	#if SAFE_SPEED_CHANGE_MODE == 0
	HESC_voidChangeDirAndSpeed(ESCS+2, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+5, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+3, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+4, ESC_DirForward, speed) ;
	
	#else
	s8 speedDesired = (s8) speed ; // all forward
	
	while(
			(((ESC_t*)(ESCS+2))->currentSpeeed) != speedDesired &&
			(((ESC_t*)(ESCS+3))->currentSpeeed) != speedDesired &&
			(((ESC_t*)(ESCS+4))->currentSpeeed) != speedDesired &&
			(((ESC_t*)(ESCS+5))->currentSpeeed) != speedDesired
		 )
	{
		for (u8 i=2; i<6; i++)
		{
			s8 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - speedDesired) ;
			deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
			if ((((ESC_t*)(ESCS+i))->currentSpeeed) < speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)+deltaSpeed) ;
			else if ((((ESC_t*)(ESCS+i))->currentSpeeed) > speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)-deltaSpeed) ;
		}
		
		if(MI2C_voidSlaveIsCalled())
			return ; //leave what ever you are doing here, and go execute new instructions.
		
		MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
	}
	#endif
}

void Backward (ESC_t* ESCS, u8 speed)
{
	#if SAFE_SPEED_CHANGE_MODE == 0
	HESC_voidChangeDirAndSpeed(ESCS+2, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+5, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+3, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+4, ESC_DirBackward, speed) ;
	
	#else
	s8 speedDesired = -((s8)speed) ; // all backward
	
	while(
			(((ESC_t*)(ESCS+2))->currentSpeeed) != speedDesired &&
			(((ESC_t*)(ESCS+3))->currentSpeeed) != speedDesired &&
			(((ESC_t*)(ESCS+4))->currentSpeeed) != speedDesired &&
			(((ESC_t*)(ESCS+5))->currentSpeeed) != speedDesired
		)
	{
		for (u8 i=2; i<6; i++)
		{
			s8 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - speedDesired) ;
			deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
			if ((((ESC_t*)(ESCS+i))->currentSpeeed) < speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)+deltaSpeed) ;
			else if ((((ESC_t*)(ESCS+i))->currentSpeeed) > speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)-deltaSpeed) ;
		}
		
		if(MI2C_voidSlaveIsCalled())
			return ; //leave what ever you are doing here, and go execute new instructions.
		
		MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
	}
	#endif
}

void Right (ESC_t* ESCS, u8 speed)
{
	#if SAFE_SPEED_CHANGE_MODE == 0
	HESC_voidChangeDirAndSpeed(ESCS+2, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+3, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+4, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+5, ESC_DirBackward, speed) ;
	
	#else
	s8 speedDesired3And4 = (s8) speed ; // both forward
	s8 speedDesired2And5 = -((s8)speed) ; // both backward
	
	
	while(
			(((ESC_t*)(ESCS+2))->currentSpeeed) != speedDesired2And5 &&
			(((ESC_t*)(ESCS+3))->currentSpeeed) != speedDesired3And4 &&
			(((ESC_t*)(ESCS+4))->currentSpeeed) != speedDesired3And4 &&
			(((ESC_t*)(ESCS+5))->currentSpeeed) != speedDesired2And5
		)
	{
		for (u8 i=2; i<6; i++)
		{
			s8 speedDesired = (i==2 || i==5) ? speedDesired2And5 : speedDesired3And4 ;
			s8 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - speedDesired) ;
			deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
			if ((((ESC_t*)(ESCS+i))->currentSpeeed) < speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)+deltaSpeed) ;
			else if ((((ESC_t*)(ESCS+i))->currentSpeeed) > speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)-deltaSpeed) ;
		}
		
		if(MI2C_voidSlaveIsCalled())
			return ; //leave what ever you are doing here, and go execute new instructions.
		
		MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
	}
	#endif
}

void Left (ESC_t* ESCS, u8 speed)
{
	#if SAFE_SPEED_CHANGE_MODE == 0
	HESC_voidChangeDirAndSpeed(ESCS+2, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+3, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+4, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+5, ESC_DirForward, speed) ;
	
	#else
	s8 speedDesired2And5 = (s8) speed ; // both forward
	s8 speedDesired3And4 = -((s8)speed) ; // both backward
	
	while(
			(((ESC_t*)(ESCS+2))->currentSpeeed) != speedDesired2And5 &&
			(((ESC_t*)(ESCS+3))->currentSpeeed) != speedDesired3And4 &&
			(((ESC_t*)(ESCS+4))->currentSpeeed) != speedDesired3And4 &&
			(((ESC_t*)(ESCS+5))->currentSpeeed) != speedDesired2And5
		)
	{
		for (u8 i=2; i<6; i++)
		{
			s8 speedDesired = (i==2 || i==5) ? speedDesired2And5 : speedDesired3And4 ;
			s8 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - speedDesired) ;
			deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
			if ((((ESC_t*)(ESCS+i))->currentSpeeed) < speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)+deltaSpeed) ;
			else if ((((ESC_t*)(ESCS+i))->currentSpeeed) > speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)-deltaSpeed) ;
		}
		
		if(MI2C_voidSlaveIsCalled())
			return ; //leave what ever you are doing here, and go execute new instructions.
		
		MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
	}
	#endif
}

void Up (ESC_t* ESCS, u8 speed)
{
	#if SAFE_SPEED_CHANGE_MODE == 0
	HESC_voidChangeDirAndSpeed(ESCS+0, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+1, ESC_DirBackward, speed) ;
	
	#else
	s8 speedDesired = -((s8)speed) ; // both backward
	
	while(
			(((ESC_t*)(ESCS+0))->currentSpeeed) != speedDesired &&
			(((ESC_t*)(ESCS+1))->currentSpeeed) != speedDesired
		)
	{
		for (u8 i=0; i<2; i++)
		{
			s8 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - speedDesired) ;
			deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
			if ((((ESC_t*)(ESCS+i))->currentSpeeed) < speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)+deltaSpeed) ;
			else if ((((ESC_t*)(ESCS+i))->currentSpeeed) > speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)-deltaSpeed) ;
		}
		
		if(MI2C_voidSlaveIsCalled())
			return ; //leave what ever you are doing here, and go execute new instructions.
		
		MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
	}
	#endif
}

void Down (ESC_t* ESCS, u8 speed)
{
	#if SAFE_SPEED_CHANGE_MODE == 0
	HESC_voidChangeDirAndSpeed(ESCS+0, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+1, ESC_DirForward, speed) ;
	
	#else
	s8 speedDesired = (s8)speed ; // both forward
	
	while(
			(((ESC_t*)(ESCS+0))->currentSpeeed) != speedDesired &&
			(((ESC_t*)(ESCS+1))->currentSpeeed) != speedDesired
		)
	{
		for (u8 i=0; i<2; i++)
		{
			s8 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - speedDesired) ;
			deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
			if ((((ESC_t*)(ESCS+i))->currentSpeeed) < speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)+deltaSpeed) ;
			else if ((((ESC_t*)(ESCS+i))->currentSpeeed) > speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)-deltaSpeed) ;
		}
		
		if(MI2C_voidSlaveIsCalled())
			return ; //leave what ever you are doing here, and go execute new instructions.
		
		MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
	}
	#endif
}

void Yaw_r (ESC_t* ESCS, u8 speed)
{
	#if SAFE_SPEED_CHANGE_MODE == 0
	HESC_voidChangeDirAndSpeed(ESCS+2, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+3, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+4, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+5, ESC_DirBackward, speed) ;
	
	#else
	s8 speedDesired2And3 = (s8) speed ; // both forward
	s8 speedDesired4And5 = -((s8)speed) ; // both backward
	
	while(
			(((ESC_t*)(ESCS+2))->currentSpeeed) != speedDesired2And3 &&
			(((ESC_t*)(ESCS+3))->currentSpeeed) != speedDesired2And3 &&
			(((ESC_t*)(ESCS+4))->currentSpeeed) != speedDesired4And5 &&
			(((ESC_t*)(ESCS+5))->currentSpeeed) != speedDesired4And5
		)
	{
		for (u8 i=2; i<6; i++)
		{
			s8 speedDesired = (i<4) ? speedDesired2And3 : speedDesired4And5 ;
			s8 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - speedDesired) ;
			deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
			if ((((ESC_t*)(ESCS+i))->currentSpeeed) < speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)+deltaSpeed) ;
			else if ((((ESC_t*)(ESCS+i))->currentSpeeed) > speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)-deltaSpeed) ;
		}
		
		if(MI2C_voidSlaveIsCalled())
			return ; //leave what ever you are doing here, and go execute new instructions.
		
		MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
	}
	#endif
}

void Yaw_l (ESC_t* ESCS, u8 speed)
{
	#if SAFE_SPEED_CHANGE_MODE == 0
	HESC_voidChangeDirAndSpeed(ESCS+2, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+3, ESC_DirBackward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+4, ESC_DirForward, speed) ;
	HESC_voidChangeDirAndSpeed(ESCS+5, ESC_DirForward, speed) ;
	
	#else
	s8 speedDesired4And5 = (s8) speed ; // both forward
	s8 speedDesired2And3 = -((s8)speed) ; // both backward
	
	while(
			(((ESC_t*)(ESCS+2))->currentSpeeed) != speedDesired2And3 &&
			(((ESC_t*)(ESCS+3))->currentSpeeed) != speedDesired2And3 &&
			(((ESC_t*)(ESCS+4))->currentSpeeed) != speedDesired4And5 &&
			(((ESC_t*)(ESCS+5))->currentSpeeed) != speedDesired4And5
		)
	{
		for (u8 i=2; i<6; i++)
		{
			s8 speedDesired = (i<4) ? speedDesired2And3 : speedDesired4And5 ;
			s8 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - speedDesired) ;
			deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
			if ((((ESC_t*)(ESCS+i))->currentSpeeed) < speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)+deltaSpeed) ;
			else if ((((ESC_t*)(ESCS+i))->currentSpeeed) > speedDesired)
				HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed)-deltaSpeed) ;
		}
		
		if(MI2C_voidSlaveIsCalled())
			return ; //leave what ever you are doing here, and go execute new instructions.
		
		MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
	}
	#endif
}

