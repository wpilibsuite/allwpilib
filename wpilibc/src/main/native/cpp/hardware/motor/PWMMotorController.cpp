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

void PWMMotorController::SetVoltage(wpi::units::volt_t output) {
  // NOLINTNEXTLINE(bugprone-integer-division)
  Set(output / RobotController::GetBatteryVoltage());
}

double PWMMotorController::Get() const {
  return GetSpeed() * (m_isInverted ? -1.0 : 1.0);
}

wpi::units::volt_t PWMMotorController::GetVoltage() const {
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

  if (m_simSpeed) {
    m_simSpeed.Set(0.0);
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
    m_simSpeed = m_simDevice.CreateDouble("Speed", true, 0.0);
    m_pwm.SetSimDevice(m_simDevice);
  }
}

WPI_UNIGNORE_DEPRECATED

void PWMMotorController::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
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

void PWMMotorController::SetSpeed(double speed) {
  if (std::isfinite(speed)) {
    speed = std::clamp(speed, -1.0, 1.0);
  } else {
    speed = 0.0;
  }

  if (m_simSpeed) {
    m_simSpeed.Set(speed);
  }

  wpi::units::microsecond_t rawValue;
  if (speed == 0.0) {
    rawValue = m_centerPwm;
  } else if (speed > 0.0) {
    rawValue = wpi::units::microsecond_t{static_cast<double>(
                   std::lround((speed * GetPositiveScaleFactor()).value()))} +
               GetMinPositivePwm();
  } else {
    rawValue = wpi::units::microsecond_t{static_cast<double>(
                   std::lround((speed * GetNegativeScaleFactor()).value()))} +
               GetMaxNegativePwm();
  }

  m_pwm.SetPulseTime(rawValue);
}

double PWMMotorController::GetSpeed() const {
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
