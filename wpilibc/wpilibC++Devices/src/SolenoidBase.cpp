/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SolenoidBase.h"

// Needs to be global since the protected resource spans all Solenoid objects.
Resource *SolenoidBase::m_allocated = NULL;

/**
 * Constructor
 * 
 * @param moduleNumber The CAN PCM ID.
 */
SolenoidBase::SolenoidBase(uint8_t moduleNumber)
	: m_moduleNumber (moduleNumber)
{
  	for (uint32_t i = 0; i < kSolenoidChannels; i++)
	{
	  void* port = getPortWithModule(moduleNumber, i);
	  int32_t status = 0;
	  m_ports[i] = initializeSolenoidPort(port, &status);
	  wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
}

/**
 * Destructor.
 */
SolenoidBase::~SolenoidBase()
{
}

/**
 * Set the value of a solenoid.
 * 
 * @param value The value you want to set on the module.
 * @param mask The channels you want to be affected.
 */
void SolenoidBase::Set(uint8_t value, uint8_t mask)
{
  	int32_t status = 0;
	for (int i = 0; i < 8; i++) { // XXX: Unhardcode
	  uint8_t local_mask = 1 << i;
	  if (mask & local_mask)
		setSolenoid(m_ports[i], value & local_mask, &status);
	}
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Read all 8 solenoids as a single byte
 * 
 * @return The current value of all 8 solenoids on the module.
 */
uint8_t SolenoidBase::GetAll()
{
  	uint8_t value = 0;
  	int32_t status = 0;
	for (int i = 0; i < 8; i++) { // XXX: Unhardcode
	  value |= getSolenoid(m_ports[i], &status) << i;
	}
	return value;
}
/**
 * Reads complete solenoid blacklist for all 8 solenoids as a single byte.
 * 
 *		If a solenoid is shorted, it is added to the blacklist and
 *		disabled until power cycle, or until faults are cleared.
 *		@see ClearAllPCMStickyFaults()
 * 
 * @return The solenoid blacklist of all 8 solenoids on the module.
 */
uint8_t SolenoidBase::GetPCMSolenoidBlackList()
{
	int32_t status = 0;
	return getPCMSolenoidBlackList(m_ports[0], &status);
}
/**
 * @return true if PCM sticky fault is set : The common 
 *			highside solenoid voltage rail is too low,
 *	 		most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageStickyFault()
{
	int32_t status = 0;
	return getPCMSolenoidVoltageStickyFault(m_ports[0], &status);
}
/**
 * @return true if PCM is in fault state : The common 
 *			highside solenoid voltage rail is too low,
 *	 		most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageFault()
{
	int32_t status = 0;
	return getPCMSolenoidVoltageFault(m_ports[0], &status);
}
/**
 * Clear ALL sticky faults inside PCM that Compressor is wired to.
 *
 * If a sticky fault is set, then it will be persistently cleared.  Compressor drive
 *		maybe momentarily disable while flags are being cleared. Care should be 
 *		taken to not call this too frequently, otherwise normal compressor 
 *		functionality may be prevented.
 *
 * If no sticky faults are set then this call will have no effect.
 */
void SolenoidBase::ClearAllPCMStickyFaults()
{
	int32_t status = 0;
	return clearAllPCMStickyFaults_sol(m_ports[0], &status);
}
