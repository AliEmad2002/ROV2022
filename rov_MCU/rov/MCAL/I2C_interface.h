/*
 * I2C_interface.h
 *
 * Created: 17/03/2022 09:51:20 م
 *  Author: Ali Emad
 */ 


#ifndef I2C_INTERFACE_H_
#define I2C_INTERFACE_H_

enum I2C_Ack {I2CAckOff, I2CAckOn} ;
	
enum MI2C_ReadWriteMode {I2CRead=1, I2CWrite, I2CGeneralCall};

// public functions :
// master functions :
void MI2C_voidMasterInit (u32 SCLFrequency) ;
void MI2C_voidMasterStart(u8 slaveAdress) ;
void MI2C_voidMasterRepeatedStart(u8 slaveAdress) ;
void MI2C_voidMasterStop(void) ;
void MI2C_voidMasterStartWait(u8 slaveAdress) ;
void MI2C_voidMasterWriteData (u8 data) ;
u8 MI2C_voidMasterReadDataNACK (void) ;
u8 MI2C_voidMasterReadDataACK (void) ;
void MI2C_voidMasterSendString(char* str) ;


// slave functions :
void MI2C_voidSlaveInit (u8 slaveAdress, u8 slaveMaskAdress, u8 generalCallResponse) ;
enum MI2C_ReadWriteMode MI2C_voidSlaveWaitForCall (void) ;
enum MI2C_ReadWriteMode MI2C_voidSlaveIsCalled (void) ;
void MI2C_voidSlaveWriteData (u8 data) ;

u8 MI2C_u8SlaveReadData(void) ;
char* MI2C_u8SlaveReadString(void) ;







#endif /* I2C_INTERFACE_H_ */