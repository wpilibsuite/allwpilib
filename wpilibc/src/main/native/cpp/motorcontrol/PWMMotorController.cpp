// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/PWMMotorController.h"

#include <fmt/format.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/RobotController.h"

using namespace frc;

void PWMMotorController::Set(double speed) {
  if (m_isInverted) {
    speed = -speed;
  }
  m_pwm.SetSpeed(speed);

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
  return m_pwm.GetSpeed() * (m_isInverted ? -1.0 : 1.0);
}

void PWMMotorController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PWMMotorController::GetInverted() const {
  return m_isInverted;
}

void PWMMotorController::Disable() {
  m_pwm.SetDisabled();
}

void PWMMotorController::StopMotor() {
  // Don't use Set(0) as that will feed the watch kitty
  m_pwm.SetSpeed(0);
}

std::string PWMMotorController::GetDescription() const {
  return fmt::format("PWM {}", GetChannel());
}

int PWMMotorController::GetChannel() const {
  return m_pwm.GetChannel();
}

void PWMMotorController::EnableDeadbandElimination(bool eliminateDeadband) {
  m_pwm.EnableDeadbandElimination(eliminateDeadband);
}

void PWMMotorController::AddFollower(PWMMotorController& follower) {
  m_nonowningFollowers.emplace_back(&follower);
}

WPI_IGNORE_DEPRECATED

PWMMotorController::PWMMotorController(std::string_view name, int channel)
    : m_pwm(channel, false) {
  wpi::SendableRegistry::AddLW(this, name, channel);
}

WPI_UNIGNORE_DEPRECATED

void PWMMotorController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { Disable(); });
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
}
