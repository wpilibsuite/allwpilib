// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/expansionhub/ExpansionHubServo.hpp"

#include "wpi/system/Errors.hpp"
#include "wpi/system/SystemServer.hpp"

using namespace wpi;

ExpansionHubServo::ExpansionHubServo(int usbId, int channel)
    : m_hub{usbId}, m_channel{channel} {
  WPILIB_AssertMessage(channel >= 0 && channel < ExpansionHub::NumServoPorts,
                       "ExHub Servo Channel {} out of range", channel);

  if (!m_hub.CheckAndReserveServo(channel)) {
    throw WPILIB_MakeError(err::ResourceAlreadyAllocated, "Channel {}",
                           channel);
  }

  m_hub.ReportUsage(fmt::format("ExHubServo[{}]", channel), "ExHubServo");

  auto systemServer = SystemServer::GetSystemServer();

  wpi::nt::PubSubOptions options;
  options.sendAll = true;
  options.keepDuplicates = true;
  options.periodic = 0.005;

  m_pulseWidthPublisher =
      systemServer
          .GetIntegerTopic(
              fmt::format("/rhsp/{}/servo{}/pulseWidth", usbId, channel))
          .Publish(options);

  m_pulseWidthPublisher.Set(1500);

  m_framePeriodPublisher =
      systemServer
          .GetIntegerTopic(
              fmt::format("/rhsp/{}/servo{}/framePeriod", usbId, channel))
          .Publish(options);

  m_framePeriodPublisher.Set(20000);

  m_enabledPublisher = systemServer
                           .GetBooleanTopic(fmt::format(
                               "/rhsp/{}/servo{}/enabled", usbId, channel))
                           .Publish(options);
}

ExpansionHubServo::~ExpansionHubServo() noexcept {
  m_hub.UnreserveServo(m_channel);
}

void ExpansionHubServo::Set(double value) {
  if (m_continousMode) {
    value = std::clamp(value, -1.0, 1.0);
    value = (value + 1.0) / 2.0;
  }

  value = std::clamp(value, 0.0, 1.0);
  if (m_reversed) {
    value = 1.0 - value;
  }
  auto rawValue = ((value * GetFullRangeScaleFactor()) + m_minPwm);
  SetPulseWidth(rawValue);
}

void ExpansionHubServo::SetAngle(wpi::units::degree_t angle) {
  angle = std::clamp(angle, m_minServoAngle, m_maxServoAngle);

  // NOLINTNEXTLINE(bugprone-integer-division)
  Set((angle - m_minServoAngle) / GetServoAngleRange());
}

void ExpansionHubServo::SetPulseWidth(wpi::units::microsecond_t pulseWidth) {
  m_pulseWidthPublisher.Set(pulseWidth.value());
}

void ExpansionHubServo::SetEnabled(bool enabled) {
  m_enabledPublisher.Set(enabled);
}

void ExpansionHubServo::SetFramePeriod(wpi::units::microsecond_t framePeriod) {
  m_framePeriodPublisher.Set(framePeriod.value());
}

wpi::units::microsecond_t ExpansionHubServo::GetFullRangeScaleFactor() {
  return m_maxPwm - m_minPwm;
}

wpi::units::degree_t ExpansionHubServo::GetServoAngleRange() {
  return m_maxServoAngle - m_minServoAngle;
}

void ExpansionHubServo::SetPWMRange(wpi::units::microsecond_t minPwm,
                                    wpi::units::microsecond_t maxPwm) {
  if (maxPwm <= minPwm) {
    throw WPILIB_MakeError(err::ParameterOutOfRange,
                           "Max PWM must be greater than Min PWM");
  }
  m_minPwm = minPwm;
  m_maxPwm = maxPwm;
}

void ExpansionHubServo::SetReversed(bool reversed) {}

void ExpansionHubServo::SetAngleRange(wpi::units::degree_t minAngle,
                                      wpi::units::degree_t maxAngle) {
  if (maxAngle <= minAngle) {
    throw WPILIB_MakeError(err::ParameterOutOfRange,
                           "Max angle must be greater than Min angle");
  }
  m_minServoAngle = minAngle;
  m_maxServoAngle = maxAngle;
}

void ExpansionHubServo::SetContinousRotationMode(bool enable) {
  m_continousMode = enable;
}
