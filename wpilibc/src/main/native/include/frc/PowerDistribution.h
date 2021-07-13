// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {

/**
 * Class for getting voltage, current, temperature, power and energy from the
 * CAN PDP.
 */
class PowerDistribution : public wpi::Sendable,
                          public wpi::SendableHelper<PowerDistribution> {
 public:
  /**
   * Constructs a PowerDistribution.
   *
   * Uses the default CAN ID (0).
   */
  PowerDistribution();

  /**
   * Constructs a PowerDistribution.
   *
   * @param module The CAN ID of the PDP
   */
  explicit PowerDistribution(int module);

  PowerDistribution(PowerDistribution&&) = default;
  PowerDistribution& operator=(PowerDistribution&&) = default;

  /**
   * Query the input voltage of the PDP.
   *
   * @return The voltage of the PDP in volts
   */
  double GetVoltage() const;

  /**
   * Query the temperature of the PDP.
   *
   * @return The temperature of the PDP in degrees Celsius
   */
  double GetTemperature() const;

  /**
   * Query the current of a single channel of the PDP.
   *
   * @return The current of one of the PDP channels (channels 0-15) in Amperes
   */
  double GetCurrent(int channel) const;

  /**
   * Query the total current of all monitored PDP channels (0-15).
   *
   * @return The the total current drawn from the PDP channels in Amperes
   */
  double GetTotalCurrent() const;

  /**
   * Query the total power drawn from the monitored PDP channels.
   *
   * @return The the total power drawn from the PDP channels in Watts
   */
  double GetTotalPower() const;

  /**
   * Query the total energy drawn from the monitored PDP channels.
   *
   * @return The the total energy drawn from the PDP channels in Joules
   */
  double GetTotalEnergy() const;

  /**
   * Reset the total energy drawn from the PDP.
   *
   * @see PowerDistribution#GetTotalEnergy
   */
  void ResetTotalEnergy();

  /**
   * Remove all of the fault flags on the PDP.
   */
  void ClearStickyFaults();

  /**
   * Gets module number (CAN ID).
   */
  int GetModule() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  hal::Handle<HAL_PDPHandle> m_handle;
  int m_module;
};

}  // namespace frc
