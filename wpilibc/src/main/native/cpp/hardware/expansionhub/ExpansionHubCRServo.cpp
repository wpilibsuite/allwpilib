// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/expansionhub/ExpansionHubCRServo.hpp"

#include "wpi/system/Errors.hpp"
#include "wpi/system/SystemServer.hpp"

using namespace wpi;

ExpansionHubCRServo::ExpansionHubCRServo(int usbId, int channel)
    : m_hub{usbId}, m_channel{channel} {
  WPILIB_AssertMessage(channel >= 0 && channel < ExpansionHub::NumServoPorts,
                       "ExHub Servo Channel {} out of range", channel);

  if (!m_hub.CheckAndReserveServo(channel)) {
    throw WPILIB_MakeError(err::ResourceAlreadyAllocated, "Channel {}",
                           channel);
  }

  m_hub.ReportUsage(fmt::format("ExHubCRServo[{}]", channel), "ExHubCRServo");

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

ExpansionHubCRServo::~ExpansionHubCRServo() noexcept {
  m_hub.UnreserveServo(m_channel);
}

void ExpansionHubCRServo::SetThrottle(double value) {
  value = std::clamp(value, -1.0, 1.0);
  value = (value + 1.0) / 2.0;
  if (m_reversed) {
    value = 1.0 - value;
  }
  auto rawValue = ((value * GetFullRangeScaleFactor()) + m_minPwm);
  SetPulseWidth(rawValue);
}

void ExpansionHubCRServo::SetPulseWidth(wpi::units::microsecond_t pulseWidth) {
  SetEnabled(true);
  m_pulseWidthPublisher.Set(pulseWidth.value());
}

void ExpansionHubCRServo::SetEnabled(bool enabled) {
  m_enabledPublisher.Set(enabled);
}

void ExpansionHubCRServo::SetFramePeriod(
    wpi::units::microsecond_t framePeriod) {
  m_framePeriodPublisher.Set(framePeriod.value());
}

wpi::units::microsecond_t ExpansionHubCRServo::GetFullRangeScaleFactor() const {
  return m_maxPwm - m_minPwm;
}

void ExpansionHubCRServo::SetPWMRange(wpi::units::microsecond_t minPwm,
                                      wpi::units::microsecond_t maxPwm) {
  if (maxPwm <= minPwm) {
    throw WPILIB_MakeError(err::ParameterOutOfRange,
                           "Max PWM must be greater than Min PWM");
  }
  m_minPwm = minPwm;
  m_maxPwm = maxPwm;
}

void ExpansionHubCRServo::SetReversed(bool reversed) {
  m_reversed = reversed;
}
