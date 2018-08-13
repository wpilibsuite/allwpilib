/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SolenoidBase.h"

#include <hal/HAL.h>
#include <hal/Solenoid.h>

using namespace frc;

int SolenoidBase::GetAll(int module) {
  int value = 0;
  int32_t status = 0;
  value = HAL_GetAllSolenoids(module, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

int SolenoidBase::GetAll() const {
  return SolenoidBase::GetAll(m_moduleNumber);
}

int SolenoidBase::GetPCMSolenoidBlackList(int module) {
  int32_t status = 0;
  return HAL_GetPCMSolenoidBlackList(module, &status);
}

int SolenoidBase::GetPCMSolenoidBlackList() const {
  return SolenoidBase::GetPCMSolenoidBlackList(m_moduleNumber);
}

bool SolenoidBase::GetPCMSolenoidVoltageStickyFault(int module) {
  int32_t status = 0;
  return HAL_GetPCMSolenoidVoltageStickyFault(module, &status);
}

bool SolenoidBase::GetPCMSolenoidVoltageStickyFault() const {
  return SolenoidBase::GetPCMSolenoidVoltageStickyFault(m_moduleNumber);
}

bool SolenoidBase::GetPCMSolenoidVoltageFault(int module) {
  int32_t status = 0;
  return HAL_GetPCMSolenoidVoltageFault(module, &status);
}

bool SolenoidBase::GetPCMSolenoidVoltageFault() const {
  return SolenoidBase::GetPCMSolenoidVoltageFault(m_moduleNumber);
}

void SolenoidBase::ClearAllPCMStickyFaults(int module) {
  int32_t status = 0;
  return HAL_ClearAllPCMStickyFaults(module, &status);
}

void SolenoidBase::ClearAllPCMStickyFaults() {
  SolenoidBase::ClearAllPCMStickyFaults(m_moduleNumber);
}

SolenoidBase::SolenoidBase(int moduleNumber) : m_moduleNumber(moduleNumber) {}
