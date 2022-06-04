/*
 * MATH.h
 *
 * Created: 17/03/2022 07:57:46 م
 *  Author: Ali Emad
 */ 


#ifndef MATH_H_
#define MATH_H_

s16 MATH_u16Abs (s16 x) ;

u16 MATH_u16GetNearestIndex (u16 x, u16* arr, u8 arrLen) ;

u16 MATH_u16GetFirstSmallerIndex (u16 x, u16* arr, u8 arrLen) ;

u16 MATH_u16GetFirstLargerIndex (u16 x, u16* arr, u8 arrLen) ;



#endif /* MATH_H_ */