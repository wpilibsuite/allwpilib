/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/Types.h>

#include "frc/SolenoidBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBuilder;

/**
 * Solenoid class for running high voltage Digital Output (PCM).
 *
 * The Solenoid class is typically used for pneumatics solenoids, but could be
 * used for any device within the current spec of the PCM.
 */
class Solenoid : public SolenoidBase,
                 public Sendable,
                 public SendableHelper<Solenoid> {
 public:
  /**
   * Constructor using the default PCM ID (0).
   *
   * @param channel The channel on the PCM to control (0..7).
   */
  explicit Solenoid(int channel);

  /**
   * Constructor.
   *
   * @param moduleNumber The CAN ID of the PCM the solenoid is attached to
   * @param channel      The channel on the PCM to control (0..7).
   */
  Solenoid(int moduleNumber, int channel);

  ~Solenoid() override;

  Solenoid(Solenoid&&) = default;
  Solenoid& operator=(Solenoid&&) = default;

  /**
   * Set the value of a solenoid.
   *
   * @param on Turn the solenoid output off or on.
   */
  virtual void Set(bool on);

  /**
   * Read the current value of the solenoid.
   *
   * @return The current value of the solenoid.
   */
  virtual bool Get() const;

  /**
   * Check if solenoid is blacklisted.
   *
   * If a solenoid is shorted, it is added to the blacklist and
   * disabled until power cycle, or until faults are cleared.
   *
   * @see ClearAllPCMStickyFaults()
   *
   * @return If solenoid is disabled due to short.
   */
  bool IsBlackListed() const;

  /**
   * Set the pulse duration in the PCM. This is used in conjunction with
   * the startPulse method to allow the PCM to control the timing of a pulse.
   * The timing can be controlled in 0.01 second increments.
   *
   * @param durationSeconds The duration of the pulse, from 0.01 to 2.55
   *                        seconds.
   *
   * @see startPulse()
   */
  void SetPulseDuration(double durationSeconds);

  /**
   * Trigger the PCM to generate a pulse of the duration set in
   * setPulseDuration.
   *
   * @see setPulseDuration()
   */
  void StartPulse();

  void InitSendable(SendableBuilder& builder) override;

 private:
  hal::Handle<HAL_SolenoidHandle> m_solenoidHandle;
  int m_channel;  // The channel on the module to control
};

}  // namespace frc
