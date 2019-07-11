/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/Types.h>

#include "frc/SolenoidBase.h"

namespace frc {

/**
 * DoubleSolenoid class for running 2 channels of high voltage Digital Output
 * (PCM).
 *
 * The DoubleSolenoid class is typically used for pneumatics solenoids that
 * have two positions controlled by two separate channels.
 */
class DoubleSolenoid : public SolenoidBase {
 public:
  enum Value { kOff, kForward, kReverse };

  /**
   * Constructor.
   *
   * Uses the default PCM ID of 0.
   *
   * @param forwardChannel The forward channel number on the PCM (0..7).
   * @param reverseChannel The reverse channel number on the PCM (0..7).
   */
  explicit DoubleSolenoid(int forwardChannel, int reverseChannel);

  /**
   * Constructor.
   *
   * @param moduleNumber   The CAN ID of the PCM.
   * @param forwardChannel The forward channel on the PCM to control (0..7).
   * @param reverseChannel The reverse channel on the PCM to control (0..7).
   */
  DoubleSolenoid(int moduleNumber, int forwardChannel, int reverseChannel);

  ~DoubleSolenoid() override;

  DoubleSolenoid(DoubleSolenoid&& rhs);
  DoubleSolenoid& operator=(DoubleSolenoid&& rhs);

  /**
   * Set the value of a solenoid.
   *
   * @param value The value to set (Off, Forward or Reverse)
   */
  virtual void Set(Value value);

  /**
   * Read the current value of the solenoid.
   *
   * @return The current value of the solenoid.
   */
  virtual Value Get() const;

  /**
   * Check if the forward solenoid is blacklisted.
   *
   * If a solenoid is shorted, it is added to the blacklist and disabled until
   * power cycle, or until faults are cleared.
   *
   * @see ClearAllPCMStickyFaults()
   * @return If solenoid is disabled due to short.
   */
  bool IsFwdSolenoidBlackListed() const;

  /**
   * Check if the reverse solenoid is blacklisted.
   *
   * If a solenoid is shorted, it is added to the blacklist and disabled until
   * power cycle, or until faults are cleared.
   *
   * @see ClearAllPCMStickyFaults()
   * @return If solenoid is disabled due to short.
   */
  bool IsRevSolenoidBlackListed() const;

  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_forwardChannel;  // The forward channel on the module to control.
  int m_reverseChannel;  // The reverse channel on the module to control.
  int m_forwardMask;     // The mask for the forward channel.
  int m_reverseMask;     // The mask for the reverse channel.
  HAL_SolenoidHandle m_forwardHandle = HAL_kInvalidHandle;
  HAL_SolenoidHandle m_reverseHandle = HAL_kInvalidHandle;
};

}  // namespace frc
