// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/motor/PWMMotorController.hpp"

#include <string>

#include <fmt/format.h>

#include "wpi/system/RobotController.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

void PWMMotorController::SetDutyCycle(double dutyCycle) {
  if (m_isInverted) {
    dutyCycle = -dutyCycle;
  }
  SetDutyCycleInternal(dutyCycle);

  for (auto& follower : m_nonowningFollowers) {
    follower->SetDutyCycle(dutyCycle);
  }
  for (auto& follower : m_owningFollowers) {
    follower->SetDutyCycle(dutyCycle);
  }

  Feed();
}

double PWMMotorController::GetDutyCycle() const {
  return GetDutyCycleInternal() * (m_isInverted ? -1.0 : 1.0);
}

wpi::units::volt_t PWMMotorController::GetVoltage() const {
  return GetDutyCycle() * RobotController::GetBatteryVoltage();
}

void PWMMotorController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PWMMotorController::GetInverted() const {
  return m_isInverted;
}

void PWMMotorController::Disable() {
  m_pwm.SetDisabled();

  if (m_simDutyCycle) {
    m_simDutyCycle.Set(0.0);
  }

  for (auto& follower : m_nonowningFollowers) {
    follower->Disable();
  }
  for (auto& follower : m_owningFollowers) {
    follower->Disable();
  }
}

void PWMMotorController::StopMotor() {
  Disable();
}

std::string PWMMotorController::GetDescription() const {
  return fmt::format("PWM {}", GetChannel());
}

int PWMMotorController::GetChannel() const {
  return m_pwm.GetChannel();
}

void PWMMotorController::EnableDeadbandElimination(bool eliminateDeadband) {
  m_eliminateDeadband = eliminateDeadband;
}

void PWMMotorController::AddFollower(PWMMotorController& follower) {
  m_nonowningFollowers.emplace_back(&follower);
}

WPI_IGNORE_DEPRECATED

PWMMotorController::PWMMotorController(std::string_view name, int channel)
    : m_pwm(channel, false) {
  wpi::util::SendableRegistry::Add(this, name, channel);

  m_simDevice = wpi::hal::SimDevice{"PWMMotorController", channel};
  if (m_simDevice) {
    m_simDutyCycle = m_simDevice.CreateDouble("DutyCycle", true, 0.0);
    m_pwm.SetSimDevice(m_simDevice);
  }
}

WPI_UNIGNORE_DEPRECATED

void PWMMotorController::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetDutyCycle(); },
      [=, this](double value) { SetDutyCycle(value); });
}

wpi::units::microsecond_t PWMMotorController::GetMinPositivePwm() const {
  if (m_eliminateDeadband) {
    return m_deadbandMaxPwm;
  } else {
    return m_centerPwm + 1_us;
  }
}

wpi::units::microsecond_t PWMMotorController::GetMaxNegativePwm() const {
  if (m_eliminateDeadband) {
    return m_deadbandMinPwm;
  } else {
    return m_centerPwm - 1_us;
  }
}

wpi::units::microsecond_t PWMMotorController::GetPositiveScaleFactor() const {
  return m_maxPwm - GetMinPositivePwm();
}

wpi::units::microsecond_t PWMMotorController::GetNegativeScaleFactor() const {
  return GetMaxNegativePwm() - m_minPwm;
}

void PWMMotorController::SetDutyCycleInternal(double dutyCycle) {
  if (std::isfinite(dutyCycle)) {
    dutyCycle = std::clamp(dutyCycle, -1.0, 1.0);
  } else {
    dutyCycle = 0.0;
  }

  if (m_simDutyCycle) {
    m_simDutyCycle.Set(dutyCycle);
  }

  wpi::units::microsecond_t rawValue;
  if (dutyCycle == 0.0) {
    rawValue = m_centerPwm;
  } else if (dutyCycle > 0.0) {
    rawValue = wpi::units::microsecond_t{static_cast<double>(std::lround(
                   (dutyCycle * GetPositiveScaleFactor()).value()))} +
               GetMinPositivePwm();
  } else {
    rawValue = wpi::units::microsecond_t{static_cast<double>(std::lround(
                   (dutyCycle * GetNegativeScaleFactor()).value()))} +
               GetMaxNegativePwm();
  }

  m_pwm.SetPulseTime(rawValue);
}

double PWMMotorController::GetDutyCycleInternal() const {
  wpi::units::microsecond_t rawValue = m_pwm.GetPulseTime();

  if (rawValue == 0_us) {
    return 0.0;
  } else if (rawValue > m_maxPwm) {
    return 1.0;
  } else if (rawValue < m_minPwm) {
    return -1.0;
  } else if (rawValue > GetMinPositivePwm()) {
    return ((rawValue - GetMinPositivePwm()) / GetPositiveScaleFactor())
        .value();
  } else if (rawValue < GetMaxNegativePwm()) {
    return ((rawValue - GetMaxNegativePwm()) / GetNegativeScaleFactor())
        .value();
  } else {
    return 0.0;
  }
}

void PWMMotorController::SetBounds(wpi::units::microsecond_t maxPwm,
                                   wpi::units::microsecond_t deadbandMaxPwm,
                                   wpi::units::microsecond_t centerPwm,
                                   wpi::units::microsecond_t deadbandMinPwm,
                                   wpi::units::microsecond_t minPwm) {
  m_maxPwm = maxPwm;
  m_deadbandMaxPwm = deadbandMaxPwm;
  m_centerPwm = centerPwm;
  m_deadbandMinPwm = deadbandMinPwm;
  m_minPwm = minPwm;
}
