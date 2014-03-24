/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef SOLENOID_BASE_H_
#define SOLENOID_BASE_H_

#include "Resource.h"
#include "SensorBase.h"
#include "HAL/HAL.h"
#include "HAL/cpp/Synchronized.h"

/**
 * SolenoidBase class is the common base class for the Solenoid and
 * DoubleSolenoid classes.
 */
class SolenoidBase : public SensorBase {
public:
	virtual ~SolenoidBase();
	uint8_t GetAll();

protected:
	explicit SolenoidBase(uint8_t moduleNumber);
	void Set(uint8_t value, uint8_t mask);
	virtual void InitSolenoid() = 0;

	uint32_t m_moduleNumber; ///< Slot number where the module is plugged into the chassis.
	static Resource *m_allocated;

private:
	void* m_ports[kSolenoidChannels];
};

#endif
