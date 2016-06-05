/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SolenoidBase.h"

#include "HAL/HAL.h"

void* SolenoidBase::m_ports[m_maxModules][m_maxPorts];
std::unique_ptr<Resource> SolenoidBase::m_allocated;

/**
 * Constructor
 *
 * @param moduleNumber The CAN PCM ID.
 */
SolenoidBase::SolenoidBase(uint8_t moduleNumber)
    : m_moduleNumber(moduleNumber) {
  for (uint32_t i = 0; i < kSolenoidChannels; i++) {
    HalPortHandle port = getPortWithModule(moduleNumber, i);
    int32_t status = 0;
    SolenoidBase::m_ports[moduleNumber][i] =
        initializeSolenoidPort(port, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    freePort(port);
  }
}

/**
 * Set the value of a solenoid.
 *
 * @param value The value you want to set on the module.
 * @param mask  The channels you want to be affected.
 */
void SolenoidBase::Set(uint8_t value, uint8_t mask, int module) {
  int32_t status = 0;
  for (int i = 0; i < m_maxPorts; i++) {
    uint8_t local_mask = 1 << i;
    if (mask & local_mask)
      setSolenoid(m_ports[module][i], value & local_mask, &status);
  }
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Read all 8 solenoids as a single byte
 *
 * @return The current value of all 8 solenoids on the module.
 */
uint8_t SolenoidBase::GetAll(int module) const {
  uint8_t value = 0;
  int32_t status = 0;
  value = getAllSolenoids(m_ports[module][0], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return value;
}

/**
 * Reads complete solenoid blacklist for all 8 solenoids as a single byte.
 *
 * If a solenoid is shorted, it is added to the blacklist and
 * disabled until power cycle, or until faults are cleared.
 * @see ClearAllPCMStickyFaults()
 *
 * @return The solenoid blacklist of all 8 solenoids on the module.
 */
uint8_t SolenoidBase::GetPCMSolenoidBlackList(int module) const {
  int32_t status = 0;
  return getPCMSolenoidBlackList(m_ports[module][0], &status);
}

/**
 * @return true if PCM sticky fault is set : The common highside solenoid
 *         voltage rail is too low, most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageStickyFault(int module) const {
  int32_t status = 0;
  return getPCMSolenoidVoltageStickyFault(m_ports[module][0], &status);
}

/**
 * @return true if PCM is in fault state : The common highside solenoid voltage
 *         rail is too low, most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageFault(int module) const {
  int32_t status = 0;
  return getPCMSolenoidVoltageFault(m_ports[module][0], &status);
}

/**
 * Clear ALL sticky faults inside PCM that Compressor is wired to.
 *
 * If a sticky fault is set, then it will be persistently cleared.  Compressor
 * drive maybe momentarily disable while flags are being cleared. Care should
 * be taken to not call this too frequently, otherwise normal compressor
 * functionality may be prevented.
 *
 * If no sticky faults are set then this call will have no effect.
 */
void SolenoidBase::ClearAllPCMStickyFaults(int module) {
  int32_t status = 0;
  return clearAllPCMStickyFaults_sol(m_ports[module][0], &status);
}
