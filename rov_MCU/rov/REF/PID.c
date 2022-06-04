/*
 * PID.c
 *
 * Created: 4/11/2022 5:41:08 PM
 *  Author: hp
 */ 


#include "STD_TYPES.h"
#include "CLOCK_interface.h"
#include "TIMER_interface.h"
#include "PID.h"

void init_PID (PID_controller* pid, f32 kp, f32 ki, f32 kd, f32 lim_min , f32 lim_max, f32 T, f32 to)
{
	pid->T = T;
	pid->to = to;
	pid->Kd = kd;
	pid->Ki = ki;
	pid->Kp = kp;
	pid->differentiator = 0;
	pid->integrator = 0;
	pid->pre_Error = 0;
	pid->pre_sensor_read = 0;
	pid->lim_max = lim_max;
	pid->lim_min = lim_min;
	pid->lastTimeStamp = 0 ;
	
	MTIM1_voidStartTickMeasure() ;
	pid->ticksInterval = T * (f32)MCLOCK_u32GetSystemClock() / MTIM1TOV1TCNT1 ;
}

f32 PID_update(PID_controller* pid, f32 setpoint , f32 sensor_read)
{
	f32 Error = setpoint - sensor_read ; 
	
	f32 proportional = Error*pid->Kp;
	f32 max_I,min_I;
	if (proportional > pid->lim_max )
		max_I =  pid->lim_max - proportional;
	else
		max_I = 0;
	if (proportional < pid->lim_min)
		min_I= pid->lim_min - proportional;
	else
		min_I = 0;
	
	if (pid->integrator > max_I)
		pid->integrator = max_I;
	else if (pid->integrator < min_I)
		pid->integrator = min_I;
	else
		pid->integrator +=  pid->Ki * 0.5 * pid->T * (Error+pid->pre_Error) ;
	
	pid->differentiator = pid->Kd*2 / (pid->T +2*pid->to) * (sensor_read - pid->pre_sensor_read) + pid->differentiator * ((2*pid->to - pid->T) /(2*pid->to + pid->T)) ;
	
	f32 out = proportional + pid->differentiator +  pid->integrator;
	if (pid->lim_max < out)
		out = pid->lim_max;
	else if (pid->lim_min > out)
		out = pid->lim_min;
	
	pid->pre_Error = Error;
	pid->pre_sensor_read = sensor_read;
	return out;

}

u8 isPID_time(PID_controller* pid)
{
	u32 current_ticks = MTIM1_u32GetElapsedTicks() ;
	if(current_ticks - pid->lastTimeStamp >= pid->ticksInterval)
	{
		pid->lastTimeStamp = current_ticks ;
		return 1 ;
	}
	else
		return 0 ;
}