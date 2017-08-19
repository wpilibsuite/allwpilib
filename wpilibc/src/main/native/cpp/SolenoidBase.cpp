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
 * @param module the module to read from
 * @return The current value of all 8 solenoids on the module.
 */
int SolenoidBase::GetAll(int module) {
  int value = 0;
  int32_t status = 0;
  value = HAL_GetAllSolenoids(module, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Read all 8 solenoids as a single byte
 *
 * @return The current value of all 8 solenoids on the module.
 */
int SolenoidBase::GetAll() const {
  return SolenoidBase::GetAll(m_moduleNumber);
}

/**
 * Reads complete solenoid blacklist for all 8 solenoids as a single byte.
 *
 * If a solenoid is shorted, it is added to the blacklist and
 * disabled until power cycle, or until faults are cleared.
 * @see ClearAllPCMStickyFaults()
 *
 * @param module the module to read from
 * @return The solenoid blacklist of all 8 solenoids on the module.
 */
int SolenoidBase::GetPCMSolenoidBlackList(int module) {
  int32_t status = 0;
  return HAL_GetPCMSolenoidBlackList(module, &status);
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
int SolenoidBase::GetPCMSolenoidBlackList() const {
  return SolenoidBase::GetPCMSolenoidBlackList(m_moduleNumber);
}

/**
 * @param module the module to read from
 * @return true if PCM sticky fault is set : The common highside solenoid
 *         voltage rail is too low, most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageStickyFault(int module) {
  int32_t status = 0;
  return HAL_GetPCMSolenoidVoltageStickyFault(module, &status);
}

/**
 * @return true if PCM sticky fault is set : The common highside solenoid
 *         voltage rail is too low, most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageStickyFault() const {
  return SolenoidBase::GetPCMSolenoidVoltageStickyFault(m_moduleNumber);
}

/**
 * @param module the module to read from
 * @return true if PCM is in fault state : The common highside solenoid voltage
 *         rail is too low, most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageFault(int module) {
  int32_t status = 0;
  return HAL_GetPCMSolenoidVoltageFault(module, &status);
}

/**
 * @return true if PCM is in fault state : The common highside solenoid voltage
 *         rail is too low, most likely a solenoid channel is shorted.
 */
bool SolenoidBase::GetPCMSolenoidVoltageFault() const {
  return SolenoidBase::GetPCMSolenoidVoltageFault(m_moduleNumber);
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
 *
 * @param module the module to read from
 */
void SolenoidBase::ClearAllPCMStickyFaults(int module) {
  int32_t status = 0;
  return HAL_ClearAllPCMStickyFaults(module, &status);
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
void SolenoidBase::ClearAllPCMStickyFaults() {
  SolenoidBase::ClearAllPCMStickyFaults(m_moduleNumber);
}
