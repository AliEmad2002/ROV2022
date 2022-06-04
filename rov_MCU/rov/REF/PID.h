/*
 * PID.h
 *
 * Created: 4/11/2022 5:40:55 PM
 *  Author: hp
 */ 


#ifndef PID_H_
#define PID_H_


typedef struct PID_controller
{
	f32 Kp; // kp * error(t)
	f32 Ki; // (ki * T/2)*(error(t) + error(t-1))+ i(n-1) 
	f32 Kd; // (kd * 2/(T+2*to)) * (error(t) - error(t-1)) + ((2*to-T)/(2*to+T))*d(n-1)
	
	f32 T ; // sample time
	f32 to; // low pass time constant
	
	u32 lastTimeStamp ;
	u32 ticksInterval ;
	
	// for the integrator path
	f32 lim_min;
	f32 lim_max;
	
	f32 integrator;
	f32 differentiator;
	
	f32 pre_Error;
	f32 pre_sensor_read;
	}PID_controller;


void init_PID (PID_controller* pid, f32 kp, f32 ki, f32 kd, f32 lim_min , f32 lim_max, f32 T, f32 to);

f32 PID_update(PID_controller* pid, f32 setpoint , f32 sensor_read);

u8 isPID_time(PID_controller* pid) ;

#endif /* PID_H_ */