﻿/*
 * bitMath.h
 *
 * Created: 12/03/2022 10:24:37 م
 *  Author: Ali Emad
 */ 


#ifndef BITMATH_H_
#define BITMATH_H_


#define SET_BIT(REG, BIT)	(REG |= 1<<BIT)
#define CLR_BIT(REG, BIT)	(REG &= ~(1<<BIT))
#define TOG_BIT(REG, BIT)	(REG ^= 1<<BIT)
#define GET_BIT(REG, BIT)	((REG>>BIT) & 1)
#define WRITE_BIT(REG, BIT, VAL)	(VAL ? SET_BIT(REG, BIT) : CLR_BIT(REG, BIT))


#endif /* BITMATH_H_ */