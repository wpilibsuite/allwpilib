/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DoubleSolenoid.h"

#include <HAL/HAL.h>
#include <HAL/Ports.h>
#include <HAL/Solenoid.h>

#include "SensorBase.h"
#include "SmartDashboard/SendableBuilder.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Constructor.
 *
 * Uses the default PCM ID of 0.
 *
 * @param forwardChannel The forward channel number on the PCM (0..7).
 * @param reverseChannel The reverse channel number on the PCM (0..7).
 */
DoubleSolenoid::DoubleSolenoid(int forwardChannel, int reverseChannel)
    : DoubleSolenoid(SensorBase::GetDefaultSolenoidModule(), forwardChannel,
                     reverseChannel) {}

/**
 * Constructor.
 *
 * @param moduleNumber   The CAN ID of the PCM.
 * @param forwardChannel The forward channel on the PCM to control (0..7).
 * @param reverseChannel The reverse channel on the PCM to control (0..7).
 */
DoubleSolenoid::DoubleSolenoid(int moduleNumber, int forwardChannel,
                               int reverseChannel)
    : SolenoidBase(moduleNumber),
      m_forwardChannel(forwardChannel),
      m_reverseChannel(reverseChannel) {
  if (!SensorBase::CheckSolenoidModule(m_moduleNumber)) {
    wpi_setWPIErrorWithContext(
        ModuleIndexOutOfRange,
        "Solenoid Module " + llvm::Twine(m_moduleNumber));
    return;
  }
  if (!SensorBase::CheckSolenoidChannel(m_forwardChannel)) {
    wpi_setWPIErrorWithContext(
        ChannelIndexOutOfRange,
        "Solenoid Channel " + llvm::Twine(m_forwardChannel));
    return;
  }
  if (!SensorBase::CheckSolenoidChannel(m_reverseChannel)) {
    wpi_setWPIErrorWithContext(
        ChannelIndexOutOfRange,
        "Solenoid Channel " + llvm::Twine(m_reverseChannel));
    return;
  }
  int32_t status = 0;
  m_forwardHandle = HAL_InitializeSolenoidPort(
      HAL_GetPortWithModule(moduleNumber, m_forwardChannel), &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumSolenoidChannels(),
                                 forwardChannel, HAL_GetErrorMessage(status));
    m_forwardHandle = HAL_kInvalidHandle;
    m_reverseHandle = HAL_kInvalidHandle;
    return;
  }

  m_reverseHandle = HAL_InitializeSolenoidPort(
      HAL_GetPortWithModule(moduleNumber, m_reverseChannel), &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumSolenoidChannels(),
                                 reverseChannel, HAL_GetErrorMessage(status));
    // free forward solenoid
    HAL_FreeSolenoidPort(m_forwardHandle);
    m_forwardHandle = HAL_kInvalidHandle;
    m_reverseHandle = HAL_kInvalidHandle;
    return;
  }

  m_forwardMask = 1 << m_forwardChannel;
  m_reverseMask = 1 << m_reverseChannel;

  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_forwardChannel,
             m_moduleNumber);
  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_reverseChannel,
             m_moduleNumber);
  SetName("DoubleSolenoid", m_moduleNumber, m_forwardChannel);
}

/**
 * Destructor.
 */
DoubleSolenoid::~DoubleSolenoid() {
  HAL_FreeSolenoidPort(m_forwardHandle);
  HAL_FreeSolenoidPort(m_reverseHandle);
}

/**
 * Set the value of a solenoid.
 *
 * @param value The value to set (Off, Forward or Reverse)
 */
void DoubleSolenoid::Set(Value value) {
  if (StatusIsFatal()) return;

  bool forward = false;
  bool reverse = false;
  switch (value) {
    case kOff:
      forward = false;
      reverse = false;
      break;
    case kForward:
      forward = true;
      reverse = false;
      break;
    case kReverse:
      forward = false;
      reverse = true;
      break;
  }
  int fstatus = 0;
  HAL_SetSolenoid(m_forwardHandle, forward, &fstatus);
  int rstatus = 0;
  HAL_SetSolenoid(m_reverseHandle, reverse, &rstatus);

  wpi_setErrorWithContext(fstatus, HAL_GetErrorMessage(fstatus));
  wpi_setErrorWithContext(rstatus, HAL_GetErrorMessage(rstatus));
}

/**
 * Read the current value of the solenoid.
 *
 * @return The current value of the solenoid.
 */
DoubleSolenoid::Value DoubleSolenoid::Get() const {
  if (StatusIsFatal()) return kOff;
  int fstatus = 0;
  int rstatus = 0;
  bool valueForward = HAL_GetSolenoid(m_forwardHandle, &fstatus);
  bool valueReverse = HAL_GetSolenoid(m_reverseHandle, &rstatus);

  wpi_setErrorWithContext(fstatus, HAL_GetErrorMessage(fstatus));
  wpi_setErrorWithContext(rstatus, HAL_GetErrorMessage(rstatus));

  if (valueForward) return kForward;
  if (valueReverse) return kReverse;
  return kOff;
}

/**
 * Check if the forward solenoid is blacklisted.
 *
 * If a solenoid is shorted, it is added to the blacklist and
 * disabled until power cycle, or until faults are cleared.
 * @see ClearAllPCMStickyFaults()
 *
 * @return If solenoid is disabled due to short.
 */
bool DoubleSolenoid::IsFwdSolenoidBlackListed() const {
  int blackList = GetPCMSolenoidBlackList(m_moduleNumber);
  return (blackList & m_forwardMask) != 0;
}

/**
 * Check if the reverse solenoid is blacklisted.
 *
 * If a solenoid is shorted, it is added to the blacklist and
 * disabled until power cycle, or until faults are cleared.
 *
 * @see ClearAllPCMStickyFaults()
 * @return If solenoid is disabled due to short.
 */
bool DoubleSolenoid::IsRevSolenoidBlackListed() const {
  int blackList = GetPCMSolenoidBlackList(m_moduleNumber);
  return (blackList & m_reverseMask) != 0;
}

void DoubleSolenoid::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Double Solenoid");
  builder.SetSafeState([=]() { Set(kOff); });
  builder.AddSmallStringProperty(
      "Value",
      [=](llvm::SmallVectorImpl<char>& buf) -> llvm::StringRef {
        switch (Get()) {
          case kForward:
            return "Forward";
          case kReverse:
            return "Reverse";
          default:
            return "Off";
        }
      },
      [=](llvm::StringRef value) {
        Value lvalue = kOff;
        if (value == "Forward")
          lvalue = kForward;
        else if (value == "Reverse")
          lvalue = kReverse;
        Set(lvalue);
      });
}
