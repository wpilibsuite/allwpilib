/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef I2C_H
#define I2C_H

#include "SensorBase.h"
#include "HAL/Semaphore.h"

class DigitalModule;

/**
 * I2C bus interface class.
 * 
 * This class is intended to be used by sensor (and other I2C device) drivers.
 * It probably should not be used directly.
 * 
 * It is constructed by calling DigitalModule::GetI2C() on a DigitalModule object.
 */
class I2C : SensorBase
{
	friend class DigitalModule;
public:
	virtual ~I2C();
	bool Transaction(uint8_t *dataToSend, uint8_t sendSize, uint8_t *dataReceived, uint8_t receiveSize);
	bool AddressOnly();
	bool Write(uint8_t registerAddress, uint8_t data);
	bool Read(uint8_t registerAddress, uint8_t count, uint8_t *data);
	void Broadcast(uint8_t registerAddress, uint8_t data);
	void SetCompatibilityMode(bool enable);

	bool VerifySensor(uint8_t registerAddress, uint8_t count, const uint8_t *expected);
private:
	static MUTEX_ID m_semaphore;
	static uint32_t m_objCount;

	I2C(DigitalModule *module, uint8_t deviceAddress);

	DigitalModule *m_module;
	uint8_t m_deviceAddress;
	bool m_compatibilityMode;
};

#endif

