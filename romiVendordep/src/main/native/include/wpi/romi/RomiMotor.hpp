// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <vector>

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

  /**
   * Make the given XRP motor follow the output of this one.
   *
   * @param follower The motor follower.
   */
  void AddFollower(RomiMotor& follower);

  /**
   * Make the given XRP motor follow the output of this one.
   *
   * @param follower The motor follower.
   */
  template <std::derived_from<RomiMotor> T>
  void AddFollower(T&& follower) {
    m_owningFollowers.emplace_back(
        std::make_unique<std::decay_t<T>>(std::forward<T>(follower)));
  } 

 private:
  hal::SimDevice m_simDevice;
  hal::SimDouble m_simSpeed;
  std::string m_deviceName;

  std::vector<RomiMotor*> m_nonowningFollowers;
  std::vector<std::unique_ptr<RomiMotor>> m_owningFollowers;

  bool m_inverted = false;
};

/** @} */

}  // namespace wpi::romi
