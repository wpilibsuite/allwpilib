// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/hal/SimDevice.hpp"
#include "wpi/hardware/motor/MotorController.hpp"
#include "wpi/hardware/motor/MotorSafety.hpp"

namespace wpi::romi {

/**
 * @defgroup romi_api Romi Hardware API
 * @{
 */

/**
 * RomiMotor.
 *
 * <p>A SimDevice based motor controller representing the motors on a Romi robot
 */
class RomiMotor : public wpi::MotorController, public wpi::MotorSafety {
 public:
  /**
   * Constructs a RomiMotor.
   *
   * @param channel The PWM channel that the RomiMotor is attached to.
   *                0 is left, 1 is right
   */
  explicit RomiMotor(int channel);

  void SetThrottle(double throttle) override;
  double GetThrottle() const override;

  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;

  void Disable() override;

  void StopMotor() override;
  std::string GetDescription() const override;

 private:
  hal::SimDevice m_simDevice;
  hal::SimDouble m_simSpeed;
  std::string m_deviceName;
  bool m_inverted = false;
};

/** @} */

}  // namespace wpi::romi
