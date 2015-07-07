/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SensorBase.h"

#include "WPIErrors.h"

const uint32_t SensorBase::kDigitalChannels;
const uint32_t SensorBase::kAnalogInputs;
const uint32_t SensorBase::kSolenoidChannels;
const uint32_t SensorBase::kSolenoidModules;
const uint32_t SensorBase::kPwmChannels;
const uint32_t SensorBase::kRelayChannels;
const uint32_t SensorBase::kPDPChannels;
const uint32_t SensorBase::kChassisSlots;
SensorBase *SensorBase::m_singletonList = nullptr;

/**
 * Creates an instance of the sensor base and gets an FPGA handle
 */
SensorBase::SensorBase()
{
}

/**
 * Add sensor to the singleton list.
 * Add this sensor to the list of singletons that need to be deleted when
 * the robot program exits. Each of the sensors on this list are singletons,
 * that is they aren't allocated directly with new, but instead are allocated
 * by the static GetInstance method. As a result, they are never deleted when
 * the program exits. Consequently these sensors may still be holding onto
 * resources and need to have their destructors called at the end of the program.
 */
void SensorBase::AddToSingletonList()
{
	m_nextSingleton = m_singletonList;
	m_singletonList = this;
}

/**
 * Delete all the singleton classes on the list.
 * All the classes that were allocated as singletons need to be deleted so
 * their resources can be freed.
 */
void SensorBase::DeleteSingletons()
{
	for (SensorBase *next = m_singletonList; next != nullptr;)
	{
		SensorBase *tmp = next;
		next = next->m_nextSingleton;
		delete tmp;
	}
	m_singletonList = nullptr;
}

/**
 * Check that the solenoid module number is valid.
 *
 * @return Solenoid module is valid and present
 */
bool SensorBase::CheckSolenoidModule(uint8_t moduleNumber)
{
	return 1 <= moduleNumber && moduleNumber <= 2; // TODO: Fix for Athena
}

/**
 * Check that the digital channel number is valid.
 * Verify that the channel number is one of the legal channel numbers. Channel numbers are
 * 1-based.
 *
 * @return Digital channel is valid
 */
bool SensorBase::CheckDigitalChannel(uint32_t channel)
{
	if (channel > 0 && channel <= kDigitalChannels)
		return true;
	return false;
}

/**
 * Check that the digital channel number is valid.
 * Verify that the channel number is one of the legal channel numbers. Channel numbers are
 * 1-based.
 *
 * @return Relay channel is valid
 */
bool SensorBase::CheckRelayChannel(uint32_t channel)
{
	if (channel > 0 && channel <= kRelayChannels)
		return true;
	return false;
}

/**
 * Check that the digital channel number is valid.
 * Verify that the channel number is one of the legal channel numbers. Channel numbers are
 * 1-based.
 *
 * @return PWM channel is valid
 */
bool SensorBase::CheckPWMChannel(uint32_t channel)
{
	if (channel > 0 && channel <= kPwmChannels)
		return true;
	return false;
}

/**
 * Check that the analog input number is valid.
 * Verify that the analog input number is one of the legal channel numbers. Channel numbers
 * are 1-based.
 *
 * @return Analog channel is valid
 */
bool SensorBase::CheckAnalogInput(uint32_t channel)
{
	if (channel > 0 && channel <= kAnalogInputs)
		return true;
	return false;
}

/**
 * Check that the analog output number is valid.
 * Verify that the analog output number is one of the legal channel numbers. Channel numbers
 * are 1-based.
 *
 * @return Analog channel is valid
 */
bool SensorBase::CheckAnalogOutput(uint32_t channel)
{
    if (channel > 0 && channel <= kAnalogOutputs)
        return true;
    return false;
}

/**
 * Verify that the solenoid channel number is within limits.
 * 
 * @return Solenoid channel is valid
 */
bool SensorBase::CheckSolenoidChannel(uint32_t channel)
{
	if (channel > 0 && channel <= kSolenoidChannels)
		return true;
	return false;
}

/**
 * Verify that the power distribution channel number is within limits.
 *
 * @return PDP channel is valid
 */
bool SensorBase::CheckPDPChannel(uint32_t channel)
{
	if (channel > 0 && channel <= kPDPChannels)
		return true;
	return false;
}
