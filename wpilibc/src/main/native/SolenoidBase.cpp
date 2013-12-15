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
 * @param moduleNumber The solenoid module (1 or 2).
 */
SolenoidBase::SolenoidBase(uint8_t moduleNumber)
	: m_moduleNumber (moduleNumber)
{
  	for (uint32_t i = 0; i < kSolenoidChannels; i++)
	{
	  void* port = getPortWithModule(moduleNumber, i+1);
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
