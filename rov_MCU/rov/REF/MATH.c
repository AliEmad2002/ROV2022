/*
 * MATH.c
 *
 * Created: 17/03/2022 07:57:29 م
 *  Author: Ali Emad
 */ 

#include "STD_TYPES.h"
#include "MATH.h"


s16 MATH_u16Abs (s16 x)
{
	return (x>0) ? x : -x ;
}

u16 MATH_u16GetNearestIndex (u16 x, u16* arr, u8 arrLen)
{
	// takes a value x, an array of values, and returns the index of the nearest value to x in that array.
	u16 deltaMin = 65535 ;
	u8 nearestIndex = 0 ;
	for (u8 i=0; i<arrLen; i++)
	{
		u16 delta = MATH_u16Abs(x - arr[i]) ;
		if (delta < deltaMin)
		{
			deltaMin = delta ;
			nearestIndex = i ;
		}
	}
	return nearestIndex ;
}

u16 MATH_u16GetFirstSmallerIndex (u16 x, u16* arr, u8 arrLen) 
{
	// takes a value x, an array of values, and returns the index of the first value smaller than x in that array.
	
	u16 firstSmaller = 0 ;
	u8 firstSmallerIndex = 0 ;
	for (u8 i=0; i<arrLen; i++)
	{
		if (arr[i] < x  &&  firstSmaller > arr[i])
		{
			firstSmaller = arr[i] ;
			firstSmallerIndex = i ;
		}
	}
	return firstSmallerIndex ;
}

u16 MATH_u16GetFirstLargerIndex (u16 x, u16* arr, u8 arrLen)
{
	// takes a value x, an array of values, and returns the index of the first value larger than x in that array.
	
	u16 firstLarger = 65535 ;
	u8 firstLargerIndex = 0 ;
	for (u8 i=0; i<arrLen; i++)
	{
		if (arr[i] > x  &&  arr[i] < firstLarger)
		{
			firstLarger = arr[i] ;
			firstLargerIndex = i ;
		}
	}
	return firstLargerIndex ;
}