/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Resource.h"
#include "SensorBase.h"
#include "HAL/HAL.hpp"
#include "HAL/cpp/Synchronized.hpp"

/**
 * SolenoidBase class is the common base class for the Solenoid and
 * DoubleSolenoid classes.
 */
class SolenoidBase : public SensorBase
{
public:
	virtual ~SolenoidBase();
	uint8_t GetAll();

	uint8_t GetPCMSolenoidBlackList();
	bool GetPCMSolenoidVoltageStickyFault();
	bool GetPCMSolenoidVoltageFault();
	void ClearAllPCMStickyFaults();
protected:
	explicit SolenoidBase(uint8_t pcmID);
	void Set(uint8_t value, uint8_t mask);
	virtual void InitSolenoid() = 0;

	uint32_t m_moduleNumber; ///< Slot number where the module is plugged into the chassis.
	static Resource *m_allocated;

private:
	void* m_ports[kSolenoidChannels];
};
