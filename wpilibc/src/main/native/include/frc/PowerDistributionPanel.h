/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/Types.h>

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBuilder;

/**
 * Class for getting voltage, current, temperature, power and energy from the
 * CAN PDP.
 */
class PowerDistributionPanel : public ErrorBase,
                               public Sendable,
                               public SendableHelper<PowerDistributionPanel> {
 public:
  PowerDistributionPanel();
  explicit PowerDistributionPanel(int module);

  PowerDistributionPanel(PowerDistributionPanel&&) = default;
  PowerDistributionPanel& operator=(PowerDistributionPanel&&) = default;

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
   * @see PowerDistributionPanel#GetTotalEnergy
   */
  void ResetTotalEnergy();

  /**
   * Remove all of the fault flags on the PDP.
   */
  void ClearStickyFaults();

  void InitSendable(SendableBuilder& builder) override;

 private:
  hal::Handle<HAL_PDPHandle> m_handle;
};

}  // namespace frc
