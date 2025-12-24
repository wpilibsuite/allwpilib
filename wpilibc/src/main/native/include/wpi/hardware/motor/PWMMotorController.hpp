// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "wpi/hal/SimDevice.h"
#include "wpi/hardware/discrete/PWM.hpp"
#include "wpi/hardware/motor/MotorController.hpp"
#include "wpi/hardware/motor/MotorSafety.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/deprecated.hpp"
#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

namespace wpi {

WPI_IGNORE_DEPRECATED

/**
 * Common base class for all PWM Motor Controllers.
 */
class PWMMotorController
    : public MotorController,
      public MotorSafety,
      public wpi::util::Sendable,
      public wpi::util::SendableHelper<PWMMotorController> {
 public:
  PWMMotorController(PWMMotorController&&) = default;
  PWMMotorController& operator=(PWMMotorController&&) = default;

  void SetDutyCycle(double dutyCycle) override;

  double GetDutyCycle() const override;

  /**
   * Gets the voltage output of the motor controller, nominally between -12 V
   * and 12 V.
   *
   * @return The voltage of the motor controller, nominally between -12 V and 12
   *     V.
   */
  virtual wpi::units::volt_t GetVoltage() const;

  void SetInverted(bool isInverted) override;

  bool GetInverted() const override;

  void Disable() override;

  // MotorSafety interface
  void StopMotor() override;
  std::string GetDescription() const override;

  int GetChannel() const;

  /**
   * Optionally eliminate the deadband from a motor controller.
   *
   * @param eliminateDeadband If true, set the motor curve on the motor
   *                          controller to eliminate the deadband in the middle
   *                          of the range. Otherwise, keep the full range
   *                          without modifying any values.
   */
  void EnableDeadbandElimination(bool eliminateDeadband);

  /**
   * Make the given PWM motor controller follow the output of this one.
   *
   * @param follower The motor controller follower.
   */
  void AddFollower(PWMMotorController& follower);

  /**
   * Make the given PWM motor controller follow the output of this one.
   *
   * @param follower The motor controller follower.
   */
  template <std::derived_from<PWMMotorController> T>
  void AddFollower(T&& follower) {
    m_owningFollowers.emplace_back(
        std::make_unique<std::decay_t<T>>(std::forward<T>(follower)));
  }

 protected:
  /**
   * Constructor for a PWM Motor %Controller connected via PWM.
   *
   * @param name Name to use for SendableRegistry
   * @param channel The PWM channel that the controller is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  PWMMotorController(std::string_view name, int channel);

  void InitSendable(wpi::util::SendableBuilder& builder) override;

  /// PWM instances for motor controller.
  PWM m_pwm;

  void SetDutyCycleInternal(double dutyCycle);
  double GetDutyCycleInternal() const;

  void SetBounds(wpi::units::microsecond_t maxPwm,
                 wpi::units::microsecond_t deadbandMaxPwm,
                 wpi::units::microsecond_t centerPwm,
                 wpi::units::microsecond_t deadbandMinPwm,
                 wpi::units::microsecond_t minPwm);

 private:
  bool m_isInverted = false;
  std::vector<PWMMotorController*> m_nonowningFollowers;
  std::vector<std::unique_ptr<PWMMotorController>> m_owningFollowers;

  wpi::hal::SimDevice m_simDevice;
  wpi::hal::SimDouble m_simDutyCycle;

  bool m_eliminateDeadband{0};
  wpi::units::microsecond_t m_minPwm{0};
  wpi::units::microsecond_t m_deadbandMinPwm{0};
  wpi::units::microsecond_t m_centerPwm{0};
  wpi::units::microsecond_t m_deadbandMaxPwm{0};
  wpi::units::microsecond_t m_maxPwm{0};

  wpi::units::microsecond_t GetMinPositivePwm() const;
  wpi::units::microsecond_t GetMaxNegativePwm() const;
  wpi::units::microsecond_t GetPositiveScaleFactor() const;
  wpi::units::microsecond_t GetNegativeScaleFactor() const;

  PWM* GetPwm() { return &m_pwm; }
};

WPI_UNIGNORE_DEPRECATED

}  // namespace wpi
