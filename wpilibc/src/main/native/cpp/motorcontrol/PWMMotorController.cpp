// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/PWMMotorController.h"

#include <string>

#include <fmt/format.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/RobotController.h"

using namespace frc;

void PWMMotorController::Set(double speed) {
  if (m_isInverted) {
    speed = -speed;
  }
  SetSpeed(speed);

  for (auto& follower : m_nonowningFollowers) {
    follower->Set(speed);
  }
  for (auto& follower : m_owningFollowers) {
    follower->Set(speed);
  }

  Feed();
}

void PWMMotorController::SetVoltage(units::volt_t output) {
  // NOLINTNEXTLINE(bugprone-integer-division)
  Set(output / RobotController::GetBatteryVoltage());
}

double PWMMotorController::Get() const {
  return GetSpeed() * (m_isInverted ? -1.0 : 1.0);
}

units::volt_t PWMMotorController::GetVoltage() const {
  return Get() * RobotController::GetBatteryVoltage();
}

void PWMMotorController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PWMMotorController::GetInverted() const {
  return m_isInverted;
}

void PWMMotorController::Disable() {
  m_pwm.SetDisabled();

  for (auto& follower : m_nonowningFollowers) {
    follower->Disable();
  }
  for (auto& follower : m_owningFollowers) {
    follower->Disable();
  }
}

void PWMMotorController::StopMotor() {
  // Don't use Set(0) as that will feed the watch kitty
  m_pwm.SetPulseTime(0_us);

  if (m_simSpeed) {
    m_simSpeed.Set(0.0);
  }

  for (auto& follower : m_nonowningFollowers) {
    follower->StopMotor();
  }
  for (auto& follower : m_owningFollowers) {
    follower->StopMotor();
  }
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
  wpi::SendableRegistry::Add(this, name, channel);

  m_simDevice = hal::SimDevice{"PWMMotorController", channel};
  if (m_simDevice) {
    m_simSpeed = m_simDevice.CreateDouble("Speed", true, 0.0);
    m_pwm.SetSimDevice(m_simDevice);
  }
}

WPI_UNIGNORE_DEPRECATED

void PWMMotorController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
}

units::microsecond_t PWMMotorController::GetMinPositivePwm() const {
  if (m_eliminateDeadband) {
    return m_deadbandMaxPwm;
  } else {
    return m_centerPwm + 1_us;
  }
}

units::microsecond_t PWMMotorController::GetMaxNegativePwm() const {
  if (m_eliminateDeadband) {
    return m_deadbandMinPwm;
  } else {
    return m_centerPwm - 1_us;
  }
}

units::microsecond_t PWMMotorController::GetPositiveScaleFactor() const {
  return m_maxPwm - GetMinPositivePwm();
}

units::microsecond_t PWMMotorController::GetNegativeScaleFactor() const {
  return GetMaxNegativePwm() - m_minPwm;
}

void PWMMotorController::SetSpeed(double speed) {
  if (std::isfinite(speed)) {
    speed = std::clamp(speed, -1.0, 1.0);
  } else {
    speed = 0.0;
  }

  if (m_simSpeed) {
    m_simSpeed.Set(speed);
  }

  units::microsecond_t rawValue;
  if (speed == 0.0) {
    rawValue = m_centerPwm;
  } else if (speed > 0.0) {
    rawValue = units::microsecond_t{static_cast<double>(std::lround(
                   (speed * GetPositiveScaleFactor()).to<double>()))} +
               GetMinPositivePwm();
  } else {
    rawValue = units::microsecond_t{static_cast<double>(std::lround(
                   (speed * GetNegativeScaleFactor()).to<double>()))} +
               GetMaxNegativePwm();
  }

  m_pwm.SetPulseTime(rawValue);
}

double PWMMotorController::GetSpeed() const {
  units::microsecond_t rawValue = m_pwm.GetPulseTime();

  if (rawValue == 0_us) {
    return 0.0;
  } else if (rawValue > m_maxPwm) {
    return 1.0;
  } else if (rawValue < m_minPwm) {
    return -1.0;
  } else if (rawValue > GetMinPositivePwm()) {
    return ((rawValue - GetMinPositivePwm()) / GetPositiveScaleFactor())
        .to<double>();
  } else if (rawValue < GetMaxNegativePwm()) {
    return ((rawValue - GetMaxNegativePwm()) / GetNegativeScaleFactor())
        .to<double>();
  } else {
    return 0.0;
  }
}

void PWMMotorController::SetBounds(units::microsecond_t maxPwm,
                                   units::microsecond_t deadbandMaxPwm,
                                   units::microsecond_t centerPwm,
                                   units::microsecond_t deadbandMinPwm,
                                   units::microsecond_t minPwm) {
  m_maxPwm = maxPwm;
  m_deadbandMaxPwm = deadbandMaxPwm;
  m_centerPwm = centerPwm;
  m_deadbandMinPwm = deadbandMinPwm;
  m_minPwm = minPwm;
}
