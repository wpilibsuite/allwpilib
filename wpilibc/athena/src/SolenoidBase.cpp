/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SolenoidBase.h"

#include "HAL/HAL.h"
#include "HAL/Solenoid.h"

using namespace frc;

/**
 * Constructor
 *
 * @param moduleNumber The CAN PCM ID.
 */
SolenoidBase::SolenoidBase(int moduleNumber) : m_moduleNumber(moduleNumber) {}

/**
 * Read all 8 solenoids as a single byte
 *
 * @return The current value of all 8 solenoids on the module.
 */
int SolenoidBase::GetAll(int module) const {
  int value = 0;
  int32_t status = 0;
  value = HAL_GetAllSolenoids(static_cast<int>(module), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
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
int SolenoidBase::GetPCMSolenoidBlackList(int module) const {
  int32_t status = 0;
  return HAL_GetPCMSolenoidBlackList(static_cast<int>(module), &status);
}

/**
 * @return true if PCM sticky fault is set : The common highside solenoid
 *         voltage rail is too low, most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageStickyFault(int module) const {
  int32_t status = 0;
  return HAL_GetPCMSolenoidVoltageStickyFault(static_cast<int>(module),
                                              &status);
}

/**
 * @return true if PCM is in fault state : The common highside solenoid voltage
 *         rail is too low, most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageFault(int module) const {
  int32_t status = 0;
  return HAL_GetPCMSolenoidVoltageFault(static_cast<int>(module), &status);
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
  return HAL_ClearAllPCMStickyFaults(static_cast<int>(module), &status);
}
