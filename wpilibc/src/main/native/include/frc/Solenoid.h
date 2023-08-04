// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <units/time.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/PneumaticsBase.h"
#include "frc/PneumaticsModuleType.h"

namespace frc {

/**
 * Solenoid class for running high voltage Digital Output on a pneumatics
 * module.
 *
 * The Solenoid class is typically used for pneumatics solenoids, but could be
 * used for any device within the current spec of the module.
 */
class Solenoid : public wpi::Sendable, public wpi::SendableHelper<Solenoid> {
 public:
  /**
   * Constructs a solenoid for a specified module and type.
   *
   * @param module The module ID to use.
   * @param moduleType The module type to use.
   * @param channel The channel the solenoid is on.
   */
  Solenoid(int module, PneumaticsModuleType moduleType, int channel);

  /**
   * Constructs a solenoid for a default module and specified type.
   *
   * @param moduleType The module type to use.
   * @param channel The channel the solenoid is on.
   */
  Solenoid(PneumaticsModuleType moduleType, int channel);

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
   * Toggle the value of the solenoid.
   *
   * If the solenoid is set to on, it'll be turned off. If the solenoid is set
   * to off, it'll be turned on.
   */
  void Toggle();

  /**
   * Get the channel this solenoid is connected to.
   */
  int GetChannel() const;

  /**
   * Check if solenoid is Disabled.
   *
   * If a solenoid is shorted, it is added to the DisabledList and
   * disabled until power cycle, or until faults are cleared.
   *
   * @see ClearAllPCMStickyFaults()
   *
   * @return If solenoid is disabled due to short.
   */
  bool IsDisabled() const;

  /**
   * Set the pulse duration in the pneumatics module. This is used in
   * conjunction with the startPulse method to allow the pneumatics module to
   * control the timing of a pulse.
   *
   * On the PCM, the timing can be controlled in 0.01 second increments, with a
   * maximum of 2.55 seconds. On the PH, the timing can be controlled in 0.001
   * second increments, with a maximum of 65.534 seconds.
   *
   * @param duration The duration of the pulse.
   *
   * @see startPulse()
   */
  void SetPulseDuration(units::second_t duration);

  /**
   * %Trigger the pneumatics module to generate a pulse of the duration set in
   * setPulseDuration.
   *
   * @see setPulseDuration()
   */
  void StartPulse();

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::shared_ptr<PneumaticsBase> m_module;
  int m_mask;
  int m_channel;
};

}  // namespace frc
