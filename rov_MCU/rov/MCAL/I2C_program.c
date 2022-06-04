/*
 * I2C_program.c
 *
 * Created: 17/03/2022 09:50:53 م
 *  Author: Ali Emad
 */

// the repeated return statement in the driver's functions may seem useless and stupid,
// but they are there for debugging

#include "bitMath.h"
#include "STD_TYPES.h"
#include "CLOCK_interface.h"
#include "I2C_private.h"
#include "I2C_interface.h"
#include <stdlib.h>
#include "manipPin_interface.h"

#include <stdio.h>



// master / slave functions :
u8 PI2C_u8GetStatus(void) // checked.
{
	return TWSR_register & 0xF8 ;
}

// master functions :
void MI2C_voidMasterInit (u32 SCLFrequency) // checked.
{
	// set frequency :
	PI2C_voidMasterSetTWBRAndPrescaler (SCLFrequency) ;

	// enable I2C :
	SET_BIT(TWCR_register, TWEN) ;
	
	MI2C_strRecived = (char*) malloc(10*sizeof(char)) ;
}

void PI2C_voidMasterSetTWBRAndPrescaler(u32 SCLFrequency) // checked.
{
	// determine best values of TWBR (8-bit value) and prescaler (1, 4, 16 or 64) :
	// frequency is better to be little less than inputed if TWBR and prescaler don't fit, than being little higher.
	// since : SCLFrequency = F_CPU / (16 + 2*TWBR*prescaler)
	// then :
	u16 productOfTWBRAndPrescaler = ( MCLOCK_u32GetSystemClock() - 16UL * SCLFrequency ) / ( 2 * SCLFrequency ) ;

	u8 prescalers[] = {1, 4, 16, 64} ;
	for (u8 i=0; i<4; i++)
	{
		u16 TWBRTheo = productOfTWBRAndPrescaler / prescalers[i] ;
		// if TWBRTheo is fitted in 8-bits, then take it with prescalers[i] :
		if (TWBRTheo < 256)
		{
			TWBR_register = (u8) TWBRTheo ;
			WRITE_BIT(TWSR_register, TWPS0, GET_BIT(prescalers[i], 0)) ;
			WRITE_BIT(TWSR_register, TWPS1, GET_BIT(prescalers[i], 1)) ;
			break ;
		}
	}
	// if the previous loop ends without finding a proper values, then the frequency inputed can't be reached by the machine, I2C won't work.
}

void MI2C_voidMasterStart(u8 slaveAdress) // checked.
{
	// remember: start is only used when master wants to start a new connection with a slave.

	// send start condition :
	SET_BIT(TWCR_register, TWSTA) ;
	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;

	// check status :
	u8 status = PI2C_u8GetStatus() ;
	if (status != I2CMasterTxStartCondSentStatus) // if start condition was not sent successfully :
		return ;

	// if start condition was sent successfully :
	// write slave address on the data bus :
	TWDR_register = slaveAdress ;

	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;

	// check status :
	status = PI2C_u8GetStatus() ;
	if (status == I2CMasterTxSlaveAdressSentWriteAckRecivedStatus) // if not (SLA+W sent, ACK received) :
		return ;

	if (status == I2CMasterTxSlaveAdressSentWriteNAckRecivedStatus) // if not (SLA+W sent, NACK received) :
		return ;
}

void MI2C_voidMasterRepeatedStart(u8 slaveAdress) // checked.
{
	// remember: repeated start is only used when master wants to read data from an already connected salve (should start connection first).

	// send start condition :
	SET_BIT(TWCR_register, TWSTA) ;
	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;

	// check status :
	u8 status = PI2C_u8GetStatus() ;
	if (status != I2CMasterRxRepeatedStartCondSentStatus) // if repeated start condition was not sent successfully :
		return ;

	// if start condition was sent successfully :
	// write slave address + R on the data bus :
	TWDR_register = slaveAdress & 1 ;

	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;

	// check status :
	status = PI2C_u8GetStatus() ;
	if (status == I2CMasterRxSlaveAdressSentReadAckRecivedStatus) // if (SLA+R sent, ACK received) :
		return ;

	// check status :
	if (status == I2CMasterRxSlaveAdressSentReadNAckRecivedStatus) // if (SLA+W sent, NACK received) :
		return ;
}

void MI2C_voidMasterStop(void) // checked.
{
	// send stop condition :
	SET_BIT(TWCR_register, TWSTO) ;
	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;
	// wait for stop condition to get sent :						why ? ain't the previus wait enough ?
	while(GET_BIT(TWCR_register, TWSTO)) ;
}

void MI2C_voidMasterStartWait(u8 slaveAdress)
{
	// like "MI2C_voidMasterStart", but pulls until the slave sends an ACK

	while(1)
	{
		// send start condition :
		SET_BIT(TWCR_register, TWSTA) ;
		// clear interrupt flag to start operation :
		SET_BIT(TWCR_register, TWINT) ;
		// wait for TWI unit to finish its current operation :
		while(!GET_BIT(TWCR_register, TWINT)) ;

		// check status :
		u8 status = PI2C_u8GetStatus() ;
		if (status != I2CMasterTxStartCondSentStatus) // if start condition was not sent successfully :
			continue ; // loop until start condition is sent successfully.

		// if start condition was sent successfully :
		// write slave address on the data bus :
		TWDR_register = slaveAdress<<1 ; // check this shift!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// clear interrupt flag to start operation :
		SET_BIT(TWCR_register, TWINT) ;
		// wait for TWI unit to finish its current operation :
		while(!GET_BIT(TWCR_register, TWINT)) ;

		// check status :
		status = PI2C_u8GetStatus() ;
		if (status != I2CMasterTxSlaveAdressSentWriteAckRecivedStatus) // if not (SLA+W sent, ACK received) :
		{
			// send top condition, and loop again until the slave of address "slaveAdress" responses with an ACK :
			MI2C_voidMasterStop() ;
			continue ;
		}

		// if the slave of address "slaveAdress" responded with an ACK :
		break ;
	}
}

void MI2C_voidMasterWriteData (u8 data) // checked.
{
	// load "data" on TWDR register (get ready for transmutation) :
	TWDR_register = data ;

	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;

	// check status :
	u8 status = PI2C_u8GetStatus() ;
	if (status == I2CMasterTxDataTransmettedAckRecivedStatus) // if (data sent, ACK received) :
		return ;

	if (status == I2CMasterTxDataTransmettedNAckRecivedStatus) // if (data sent, NACK received) :
		return ;
}

u8 MI2C_voidMasterReadDataNACK (void) // checked.
{
	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;

	// return value of TWDR register (received data) :
	return TWDR_register ;
}

u8 MI2C_voidMasterReadDataACK (void) // checked.
{
	// enable ACK :
	SET_BIT(TWCR_register, TWEA) ;
	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;

	// return value of TWDR register (received data) :
	return TWDR_register ;
}

void MI2C_voidMasterSendString(char* str)
{
	for(u8 i=0; str[i]!='\0'; i++)
		MI2C_voidMasterWriteData(str[i]) ;

	MI2C_voidMasterWriteData('\0') ;
}


// slave functions : response
void MI2C_voidSlaveInit (u8 slaveAdress, u8 slaveMaskAdress, u8 generalCallResponse) // checked.
{
	// write slaveAdress :
	// remember that addresses 111-xxxx and 000-xxxx are reserved by default of I2C.
	TWAR_register = slaveAdress<<1 ;
	
	if (generalCallResponse)
		SET_BIT(TWAR_register, TWGCE) ;

	// write slaveMaskAdress :
	TWAMR_register = slaveMaskAdress ;

	// enable TWI unit :
	SET_BIT(TWCR_register, TWEN) ;
	// send ACK :
	SET_BIT(TWCR_register, TWEA) ;
	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	//while(!GET_BIT(TWCR_register, TWINT)) ; // this blocks the controller when no master is available, so do a delay:
	MCLOCK_voidDelayMS(20) ;
	
	
	MI2C_strRecived = (char*) malloc(10*sizeof(char)) ;
	was_called = 0 ;
}

enum MI2C_ReadWriteMode MI2C_voidSlaveWaitForCall (void) // checked.
{
	if (was_called)
	{
		was_called = 0 ;
		return 1 ;
	}
	while(1)
	{
		// wait for interrupt flag to be set by hardware (is set when own slave address is called on the data bus) :
		while(!GET_BIT(TWCR_register, TWINT)) ;

		// check status :
		u8 status = PI2C_u8GetStatus() ;
		
		if (status == I2CSlaveRxAdressRecivedWriteAckStatus	||	status == I2CSlaveRxAttribLostAdressRecivedWriteAckStatus) // if Own SLA+W has been received, ACK has been returned
			return I2CWrite ;

		if (status == I2CSlaveTxAdressRecivedReadAckStatus	||	status == I2CSlaveTxAttribLostAdressRecivedReadAckStatus) // if Own SLA+R has been received, ACK has been returned
			return I2CRead ;
		
		if (status == I2CSlaveRxGeneralCallAckStatus	||	status == I2CSlaveRxAttribLostGeneralCallAckStatus || status==0x98 || status==0x90 || status==0x80 || status==0x88) // if general call has been received, ACK has been returned
			return status ;
	}
	
}

enum MI2C_ReadWriteMode MI2C_voidSlaveIsCalled (void)
{
	// no wait here !
		
	// check status :
	u8 status = PI2C_u8GetStatus() ;
	
	if (status == I2CSlaveRxAdressRecivedWriteAckStatus	||	status == I2CSlaveRxAttribLostAdressRecivedWriteAckStatus) // if Own SLA+W has been received, ACK has been returned
	{
		was_called = 1 ;
		return I2CWrite ;
	}

	if (status == I2CSlaveTxAdressRecivedReadAckStatus	||	status == I2CSlaveTxAttribLostAdressRecivedReadAckStatus) // if Own SLA+R has been received, ACK has been returned
	{
		was_called = 1 ;
		return I2CRead ;
	}
	
	if (status == I2CSlaveRxGeneralCallAckStatus	||	status == I2CSlaveRxAttribLostGeneralCallAckStatus || status==0x98 || status==0x90 || status==0x80 || status==0x88) // if general call has been received, ACK has been returned
	{
		was_called = 1 ;
		return status ;
	}
		
	else
		return 0 ; // hasn't been called.
}

void MI2C_voidSlaveWriteData (u8 data) // checked.
{
	// load "data" to TWDR register (get ready for transmission) :
	TWDR_register = data ;

	// enable ACK :
	SET_BIT(TWCR_register, TWEA) ;
	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;
	
	

	// check status :
	u8 status = PI2C_u8GetStatus() ;

	if (status == I2CSlaveTxStopOrRepStartStatus) // if stop or repeated start condition was received :
		return ;

	if (status == I2CSlaveTxDataTransmittedAckRecievedStatus) // if data transmitted, ACK received :
		return ;

	if (status == I2CSlaveTxDataTransmittedNAckRecievedStatus) // if data transmitted, NACK received :
		return ;

	if (status == I2CSlaveTxLastBytteTransmittedAckRecievedTWEA0Status) // if Last data byte in TWDR has been transmitted (TWEA = 0), ACK has been received		return ;
}

u8 MI2C_u8SlaveReadData(void) // checked.
{
	// blocks until a byte is received.

	// enable TWI :
	SET_BIT(TWCR_register, TWEN) ;
	// enable ACK :
	SET_BIT(TWCR_register, TWEA) ;
	// clear interrupt flag to start operation :
	SET_BIT(TWCR_register, TWINT) ;
	// wait for TWI unit to finish its current operation :
	while(!GET_BIT(TWCR_register, TWINT)) ;
	
	// check status :
	u8 status = PI2C_u8GetStatus() ;

	if (status == I2CSlaveRxDataRecievedAckReturnedStatus || status == I2CSlaveRxDataRecievedAckReturnedGeneralStatus) // if data received, ACK returned :
		return TWDR_register ;

	if (status == I2CSlaveRxDataRecievedNAckReturnedStatus || status == I2CSlaveRxDataRecievedNAckReturnedGeneralStatus) // if data received, NACK returned :
		return TWDR_register ;

	if (status == I2CSlaveTxStopOrRepStartStatus) // if stop or repeated start condition was received :
	{
		// clear interrupt flag to start operation :
		SET_BIT(TWCR_register, TWINT) ;
		return '\0' ;
	}
	

	// if none of the previous :
	return '\0' ;
}

char* MI2C_u8SlaveReadString(void)
{
	char recievedChar = (char)MI2C_u8SlaveReadData() ;
	u8 i ;
	for(i=0; recievedChar!='\0'; i++)
	{
		MI2C_strRecived[i] = recievedChar ;
		recievedChar = (char)MI2C_u8SlaveReadData() ;
	}
	MI2C_strRecived[i] = '\0' ;
	return MI2C_strRecived ;
}
