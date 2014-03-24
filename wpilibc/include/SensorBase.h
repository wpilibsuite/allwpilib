/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef SENSORBASE_H_
#define SENSORBASE_H_

#include "ErrorBase.h"
#include <stdio.h>
#include "Base.h"

/**
 * Base class for all sensors.
 * Stores most recent status information as well as containing utility functions for checking
 * channels and error processing.
 */
class SensorBase: public ErrorBase {
public:
	SensorBase();
	virtual ~SensorBase();
	static void DeleteSingletons();
	static uint32_t GetDefaultAnalogModule() { return 1; }
	static uint32_t GetDefaultDigitalModule() { return 1; }
	static uint32_t GetDefaultSolenoidModule() { return 1; }
	static bool CheckAnalogModule(uint8_t moduleNumber);
	static bool CheckDigitalModule(uint8_t moduleNumber);
	static bool CheckPWMModule(uint8_t moduleNumber);
	static bool CheckRelayModule(uint8_t moduleNumber);
	static bool CheckSolenoidModule(uint8_t moduleNumber);
	static bool CheckDigitalChannel(uint32_t channel);
	static bool CheckRelayChannel(uint32_t channel);
	static bool CheckPWMChannel(uint32_t channel);
	static bool CheckAnalogChannel(uint32_t channel);
	static bool CheckSolenoidChannel(uint32_t channel);

	static const uint32_t kDigitalChannels = 14;
	static const uint32_t kAnalogChannels = 8;
	static const uint32_t kAnalogModules = 2;
	static const uint32_t kDigitalModules = 2;
	static const uint32_t kSolenoidChannels = 8;
	static const uint32_t kSolenoidModules = 2;
	static const uint32_t kPwmChannels = 10;
	static const uint32_t kRelayChannels = 8;
	static const uint32_t kChassisSlots = 8;
protected:
	void AddToSingletonList();

private:
	DISALLOW_COPY_AND_ASSIGN(SensorBase);
	static SensorBase *m_singletonList;
	SensorBase *m_nextSingleton;
};


#endif
