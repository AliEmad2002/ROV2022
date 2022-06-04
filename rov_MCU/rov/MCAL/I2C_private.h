/*
 * I2C_private.h
 *
 * Created: 17/03/2022 09:51:07 م
 *  Author: Ali Emad
 */ 


#ifndef I2C_PRIVATE_H_
#define I2C_PRIVATE_H_

// registers and thier bits :
#define TWAMR_register ( *(volatile u8*) 0xBD )

#define TWCR_register ( *(volatile u8*) 0xBC )
#define TWIE 0
#define TWEN 2
#define TWWC 3
#define TWSTO 4
#define TWSTA 5
#define TWEA 6
#define TWINT 7

#define TWDR_register ( *(volatile u8*) 0xBB )

#define TWAR_register ( *(volatile u8*) 0xBA )
#define TWGCE 0

#define TWSR_register ( *(volatile u8*) 0xB9 )
#define TWPS0 0
#define TWPS1 1
#define TWS3 3
#define TWS4 4
#define TWS5 5
#define TWS6 6
#define TWS7 7

#define TWBR_register ( *(volatile u8*) 0xB8 )


// private functions :
u8 PI2C_u8GetStatus(void) ;
void PI2C_voidMasterSetTWBRAndPrescaler(u32 SCLFrequency) ;

// private variables :
char* MI2C_strRecived ;
u8 was_called ;

// status values :
#define I2CMasterTxStartCondSentStatus 0x08
#define I2CMasterTxSlaveAdressSentWriteAckRecivedStatus 0x18
#define I2CMasterTxSlaveAdressSentWriteNAckRecivedStatus 0x20
#define I2CMasterTxDataTransmettedAckRecivedStatus 0x28
#define I2CMasterTxDataTransmettedNAckRecivedStatus 0x30

#define I2CMasterRxSlaveAdressSentReadAckRecivedStatus 0x40
#define I2CMasterRxSlaveAdressSentReadNAckRecivedStatus 0x48
#define I2CMasterRxRepeatedStartCondSentStatus 0x10

#define I2CSlaveRxAdressRecivedWriteAckStatus 0x60
#define I2CSlaveRxAttribLostAdressRecivedWriteAckStatus 0x68
#define I2CSlaveRxGeneralCallAckStatus 0x70
#define I2CSlaveRxAttribLostGeneralCallAckStatus 0x78
#define I2CSlaveRxDataRecievedAckReturnedStatus 0x80
#define I2CSlaveRxDataRecievedAckReturnedGeneralStatus 0x90
#define I2CSlaveRxDataRecievedNAckReturnedStatus 0x80
#define I2CSlaveRxDataRecievedNAckReturnedGeneralStatus 0x90

#define I2CSlaveTxStopOrRepStartStatus 0xA0
#define I2CSlaveTxAdressRecivedReadAckStatus 0xA8
#define I2CSlaveTxAttribLostAdressRecivedReadAckStatus 0xB0
#define I2CSlaveTxDataTransmittedAckRecievedStatus 0xB8
#define I2CSlaveTxDataTransmittedNAckRecievedStatus 0xC0
#define I2CSlaveTxLastBytteTransmittedAckRecievedTWEA0Status 0xC8






#endif /* I2C_PRIVATE_H_ */
