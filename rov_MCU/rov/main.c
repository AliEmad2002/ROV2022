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
	
	2 left_forwad motor pin PD3
	3 left_backward motor pin PB3
	
	4 right_backward motor pin PB2
	5 right_forwad motor pin PB1
	
	assuming that the forward direction is that which makes the flow exit from the thin-looking side of the thruster !
*/

#include "STD_TYPES.h"
#include "bitMath.h"
#include "ESC_interface.h"
#include "CLOCK_interface.h"
#include "I2C_interface.h"
#include "manipPin_interface.h"
#include "PID.h"
#include "LCD_interface.h"
#include "TIMER_interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
	
#define SLAVE_ADRR 0x03

void Init_ESCs (ESC_t* ESCS) ;
void ChangeAllSpeeds(ESC_t* ESCS, s16 speedForward, s16 speedRight, s16 speedYawRight) ;
void Up (ESC_t* ESCS, u8 speed);
void Down (ESC_t* ESCS, u8 speed);
void check_indexing(ESC_t* ESCS) ;
f32 correct_angle(f32 angle360) ;

u8 SAFE_SPEED_CHANGE_MODE ;
u8 CHANGE_SPEED_DELAY ; //ms

int main(void)
{
	MCLOCK_voidDelayMS(6000) ; // wait for ESC's to get powered and stable.
	
	// init them :
	ESC_t* ESCS = (ESC_t*)malloc(6*sizeof(ESC_t)) ;
	Init_ESCs(ESCS) ;
	
	// init I2C :
	MI2C_voidSlaveInit(SLAVE_ADRR, 0, 1) ;
	char* data ;
	char key ;
	u8 value ; // must be +ve, and of 3 digits !
	
	// init Switches : (all off at first) :
	SAFE_SPEED_CHANGE_MODE = 1 ;
	CHANGE_SPEED_DELAY = 8 ; //ms
	u8 ALTITUDE_PID_ON = 0 ;
	u8 ROLL_PID_ON = 0 ;
	u8 YAW_PID_ON = 0 ;
	
	// init PID :
	f32 ALTITUDE_Kp = 0.5f ;
	f32 ALTITUDE_Ki = 0.5f ;
	f32 ALTITUDE_Kd = 0.5f ;
	f32 ALTITUDE_REFRESH_RATE = 0.03f ; // sec
	f32 altitudeCurrent = 0 ; // cm
	f32 altitudeRequiered = 0 ; // cm
	PID_controller altitudePID ;
	if (ALTITUDE_PID_ON == 1)
		init_PID(&altitudePID, ALTITUDE_Kp, ALTITUDE_Ki, ALTITUDE_Kd, -100.0f, 100.0f, ALTITUDE_REFRESH_RATE, 0.001f) ;
	
	f32 ROLL_Kp = 0.5f ;
	f32 ROLL_Ki = 0.2f ;
	f32 ROLL_Kd = 0.5f ;
	f32 ROLL_REFRESH_RATE = 0.03f ; // sec
	f32 thetaRollCurrent = 0 ; // degree
	f32 thetaRollRequiered = 0 ; // degree
	PID_controller rollPID ;
	if (ROLL_PID_ON == 1)
		init_PID(&rollPID, ROLL_Kp, ROLL_Ki, ROLL_Kd, -100.0f, 100.0f, ROLL_REFRESH_RATE, 0.001f) ;
	
	f32 YAW_Kp = 0.5f ;
	f32 YAW_Ki = 0.5f ;
	f32 YAW_Kd = 0.5f ;
	f32 YAW_REFRESH_RATE = 0.03f ; // sec
	f32 thetaYawCurrent = 0 ; // degree
	f32 thetaYawRequiered = 0 ; // degree
	PID_controller yawPID ;
	if (YAW_PID_ON == 1)
		init_PID(&yawPID, YAW_Kp, YAW_Ki, YAW_Kd, -100.0f, 100.0f, YAW_REFRESH_RATE, 0.001f) ;
	
	s16 speedForward = 0 ;
	s16 speedRight = 0 ;
	s16 speedYawRight = 0 ;
	
    while(1)
    {	
		if(MI2C_voidSlaveIsCalled()) //Rpi has sent new data :
		{
			data = MI2C_u8SlaveReadString();
			key = data[0];
			value = (u8)(data[1]-'0')*100 + (u8)(data[2]-'0')*10 + (u8)(data[3]-'0');
			
			// Movement :
			if (key=='U')
				Up(ESCS, value);
			else if (key=='D')
				Down(ESCS, value);
			else if (key=='F')
			{
				speedForward = value ;
				ChangeAllSpeeds(ESCS, speedForward, speedRight, speedYawRight) ;
			}
			else if (key=='B')
			{
				speedForward = -value ;
				ChangeAllSpeeds(ESCS, speedForward, speedRight, speedYawRight) ;
			}
			else if (key=='R')
			{
				speedRight = value ;
				ChangeAllSpeeds(ESCS, speedForward, speedRight, speedYawRight) ;
			}
			else if (key=='L')
			{
				speedRight = -value ;
				ChangeAllSpeeds(ESCS, speedForward, speedRight, speedYawRight) ;
			}
			else if (key=='X')
			{
				speedYawRight = value ;
				ChangeAllSpeeds(ESCS, speedForward, speedRight, speedYawRight) ;
			}
			else if (key=='Y')
			{
				speedYawRight = -value ;
				ChangeAllSpeeds(ESCS, speedForward, speedRight, speedYawRight) ;
			}
			else if (key=='m')
				check_indexing(ESCS) ;
			else if (key=='j')
				SAFE_SPEED_CHANGE_MODE = value % 2 ;
			else if (key=='J')
				CHANGE_SPEED_DELAY = value ;
			else if (key=='r')
				Init_ESCs(ESCS) ;
			
			// PID :
			else if (key=='n')
				ALTITUDE_PID_ON = value % 2 ;
			else if (key=='Z') // user wants ROV to go to that altitude
				altitudeRequiered = (f32)value ;
			else if (key=='z') // sensor reading of current ROV's altitude
				altitudeCurrent = (f32)value ;
			else if (key=='b')
				ROLL_PID_ON = value % 2 ;
			else if (key=='Q') // user wants ROV to make that theta_roll
				thetaRollRequiered = correct_angle((f32)value) ;
			else if (key=='q') // sensor reading of current ROV's theta_roll
			{
				thetaRollCurrent = correct_angle((f32)value) ;
				//f32 speed = PID_update(&rollPID, thetaRollRequiered, thetaRollCurrent) ;
				//HESC_voidChangeSpeed(ESCS+1, (ESCS+1)->currentSpeeed + (s16)speed) ;
			}
			else if (key=='A')
			{
				ROLL_Kp = (f32)value / 100.0f ;
				init_PID(&rollPID, ROLL_Kp, ROLL_Ki, ROLL_Kd, -100.0f, 100.0f, ROLL_REFRESH_RATE, 0.001f) ;
			}
			else if (key=='S')
			{
				ROLL_Ki = (f32)value / 100.0f ;
				init_PID(&rollPID, ROLL_Kp, ROLL_Ki, ROLL_Kd, -100.0f, 100.0f, ROLL_REFRESH_RATE, 0.001f) ;
			}
			else if (key=='G')
			{
				ROLL_Kd = (f32)value / 100.0f ;
				init_PID(&rollPID, ROLL_Kp, ROLL_Ki, ROLL_Kd, -100.0f, 100.0f, ROLL_REFRESH_RATE, 0.001f) ;
			}
			else if (key=='E')
			{
				ROLL_REFRESH_RATE = (f32)value / 1000.0f ;
				init_PID(&rollPID, ROLL_Kp, ROLL_Ki, ROLL_Kd, -100.0f, 100.0f, ROLL_REFRESH_RATE, 0.001f) ;
			}
			else if (key=='c')
				YAW_PID_ON = value % 2 ;
			else if (key=='w') // user wants ROV to make that theta_roll
				thetaYawRequiered = correct_angle((f32)value) ;
			else if (key=='o') // sensor reading of current ROV's theta_roll
				thetaYawCurrent = correct_angle((f32)value) ;
			else if (key=='T')
			{
				YAW_Kp = (f32)value / 100.0f ;
				init_PID(&yawPID, YAW_Kp, YAW_Ki, YAW_Kd, -100.0f, 100.0f, YAW_REFRESH_RATE, 0.001f) ;
			}
			else if (key=='O')
			{
				YAW_Ki = (f32)value / 100.0f ;
				init_PID(&yawPID, YAW_Kp, YAW_Ki, YAW_Kd, -100.0f, 100.0f, YAW_REFRESH_RATE, 0.001f) ;
			}
			else if (key=='P')
			{
				YAW_Kd = (f32)value / 100.0f ;
				init_PID(&yawPID, YAW_Kp, YAW_Ki, YAW_Kd, -100.0f, 100.0f, YAW_REFRESH_RATE, 0.001f) ;
			}
			else if (key=='K')
			{
				YAW_REFRESH_RATE = (f32)value / 1000.0f ;
				init_PID(&yawPID, YAW_Kp, YAW_Ki, YAW_Kd, -100.0f, 100.0f, YAW_REFRESH_RATE, 0.001f) ;
			}
			
		}
		
		// refreshing motors speeds :
		if (SAFE_SPEED_CHANGE_MODE == 0)
		{
			for (u8 i=0; i<6; i++)
			{
				if ((((ESC_t*)(ESCS+i))->targetSpeed) != (((ESC_t*)(ESCS+i))->currentSpeeed))
					HESC_voidChangeSpeed(ESCS+i, ((ESC_t*)(ESCS+i))->targetSpeed) ;
			}
		}
		else
		{
			for (u8 i=0; i<6; i++)
			{
				if ((((ESC_t*)(ESCS+i))->targetSpeed) != (((ESC_t*)(ESCS+i))->currentSpeeed))
				{
					s16 deltaSpeed = abs((((ESC_t*)(ESCS+i))->currentSpeeed) - (((ESC_t*)(ESCS+i))->targetSpeed)) ;
					deltaSpeed = (deltaSpeed<10) ? 1 : 10 ;
					if ((((ESC_t*)(ESCS+i))->targetSpeed) > (((ESC_t*)(ESCS+i))->currentSpeeed))
						HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed) + deltaSpeed) ;
					else
						HESC_voidChangeSpeed(ESCS+i, (((ESC_t*)(ESCS+i))->currentSpeeed) - deltaSpeed) ;
				}
			}
			MCLOCK_voidDelayMS(CHANGE_SPEED_DELAY) ;
		}
		
		// PID :
		if (ALTITUDE_PID_ON == 1)
			if (isPID_time(&altitudePID))
			{
				f32 speed = PID_update(&altitudePID, altitudeRequiered, altitudeCurrent) ;
				HESC_voidChangeSpeed(ESCS+0, (((ESC_t*)(ESCS+0))->currentSpeeed) + (s16)speed) ;
			}
		
		if (ROLL_PID_ON == 1)
			if (isPID_time(&rollPID))
			{
				f32 speed = PID_update(&rollPID, thetaRollRequiered, thetaRollCurrent) ;
				HESC_voidChangeSpeed(ESCS+1, (((ESC_t*)(ESCS+1))->currentSpeeed) + (s16)speed) ;
			}
		
		if (YAW_PID_ON == 1)
			if (isPID_time(&yawPID))
			{
				f32 speed = PID_update(&yawPID, thetaYawRequiered, thetaYawCurrent) ;
				HESC_voidChangeSpeed(ESCS+2, (((ESC_t*)(ESCS+2))->currentSpeeed) + (s16)speed) ;
				HESC_voidChangeSpeed(ESCS+5, (((ESC_t*)(ESCS+5))->currentSpeeed) - (s16)speed) ;
			}
    }
}

void Init_ESCs (ESC_t* ESCS)
{
	HESC_voidInit(ESCS+0, ESC_Timer0, ESC_ChannelA, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+1, ESC_Timer0, ESC_ChannelB, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+2, ESC_Timer2, ESC_ChannelB, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+3, ESC_Timer2, ESC_ChannelA, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+4, ESC_Timer1, ESC_ChannelB, 1100, 1500, 1900, 400) ;
	HESC_voidInit(ESCS+5, ESC_Timer1, ESC_ChannelA, 1100, 1500, 1900, 400) ;
	#if DEBUG_MODE == 0
	MCLOCK_voidDelayMS(7000) ; // wait for ESC's to get init.
	#endif
}

void ChangeAllSpeeds(ESC_t* ESCS, s16 speedForward, s16 speedRight, s16 speedYawRight)
{
	((ESC_t*)(ESCS+2))->targetSpeed = speedForward + speedRight + speedYawRight ;
	((ESC_t*)(ESCS+3))->targetSpeed = -speedForward + speedRight - speedYawRight ;
	((ESC_t*)(ESCS+4))->targetSpeed = -speedForward - speedRight + speedYawRight ;
	((ESC_t*)(ESCS+5))->targetSpeed = speedForward - speedRight - speedYawRight ;
	for (u8 i=2; i<6; i++)
	{
		if ((((ESC_t*)(ESCS+i))->targetSpeed) > 100)
			(((ESC_t*)(ESCS+i))->targetSpeed) = 100 ;
		else if ((((ESC_t*)(ESCS+i))->targetSpeed) < -100)
			(((ESC_t*)(ESCS+i))->targetSpeed) = -100 ;
	}
}

void Up (ESC_t* ESCS, u8 speed)
{
	((ESC_t*)(ESCS+0))->targetSpeed = -speed ;
	((ESC_t*)(ESCS+1))->targetSpeed = -speed ;
}

void Down (ESC_t* ESCS, u8 speed)
{
	((ESC_t*)(ESCS+0))->targetSpeed = speed ;
	((ESC_t*)(ESCS+1))->targetSpeed = speed ;
}

void check_indexing(ESC_t* ESCS)
{
	for (u8 i=0; i<6; i++)
	{
		HESC_voidChangeSpeed(ESCS+i, 70) ;
		MCLOCK_voidDelayMS(1000) ;
		HESC_voidChangeSpeed(ESCS+i, 0) ;
		MCLOCK_voidDelayMS(1000) ;
	}
}

f32 correct_angle(f32 angle360)
{
	if (angle360 > 180.0f)
		return angle360 - 360.0f ;
	return angle360 ;
}