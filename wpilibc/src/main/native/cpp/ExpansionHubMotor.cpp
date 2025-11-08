// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/ExpansionHubMotor.hpp"

#include "wpi/system/Errors.hpp"
#include "wpi/system/SystemServer.hpp"

static constexpr int kPercentageMode = 0;
static constexpr int kVoltageMode = 1;
static constexpr int kPositionMode = 2;
static constexpr int kVelocityMode = 3;
static constexpr int kFollowerMode = 4;

using namespace wpi;

ExpansionHubMotor::ExpansionHubMotor(int usbId, int channel)
    : m_hub{usbId},
      m_channel{channel},
      m_velocityPidConstants{usbId, channel, true},
      m_positionPidConstants{usbId, channel, false} {
  FRC_AssertMessage(channel >= 0 && channel < ExpansionHub::NumMotorPorts,
                    "ExHub Motor Channel {} out of range", channel);

  if (!m_hub.CheckAndReserveMotor(channel)) {
    throw FRC_MakeError(err::ResourceAlreadyAllocated, "Channel {}", channel);
  }

  m_hub.ReportUsage(fmt::format("ExHubServo[{}]", channel), "ExHubServo");

  auto systemServer = SystemServer::GetSystemServer();

  wpi::nt::PubSubOptions options;
  options.sendAll = true;
  options.keepDuplicates = true;
  options.periodic = 0.005;

  m_encoderSubscriber = systemServer
                            .GetDoubleTopic(fmt::format(
                                "/rhsp/{}/motor{}/encoder", usbId, channel))
                            .Subscribe(0, options);
  m_encoderVelocitySubscriber =
      systemServer
          .GetDoubleTopic(
              fmt::format("/rhsp/{}/motor{}/encoderVelocity", usbId, channel))
          .Subscribe(0, options);
  m_currentSubscriber = systemServer
                            .GetDoubleTopic(fmt::format(
                                "/rhsp/{}/motor{}/current", usbId, channel))
                            .Subscribe(0, options);

  m_setpointPublisher = systemServer
                            .GetDoubleTopic(fmt::format(
                                "/rhsp/{}/motor{}/setpoint", usbId, channel))
                            .Publish(options);

  m_distancePerCountPublisher =
      systemServer
          .GetDoubleTopic(
              fmt::format("/rhsp/{}/motor{}/distancePerCount", usbId, channel))
          .Publish(options);

  m_floatOn0Publisher = systemServer
                            .GetBooleanTopic(fmt::format(
                                "/rhsp/{}/motor{}/floatOn0", usbId, channel))
                            .Publish(options);
  m_enabledPublisher = systemServer
                           .GetBooleanTopic(fmt::format(
                               "/rhsp/{}/motor{}/enabled", usbId, channel))
                           .Publish(options);

  m_modePublisher =
      systemServer
          .GetIntegerTopic(fmt::format("/rhsp/{}/motor{}/mode", usbId, channel))
          .Publish(options);

  m_reversedPublisher = systemServer
                            .GetBooleanTopic(fmt::format(
                                "/rhsp/{}/motor{}/reversed", usbId, channel))
                            .Publish(options);

  m_resetEncoderPublisher =
      systemServer
          .GetBooleanTopic(
              fmt::format("/rhsp/{}/motor{}/resetEncoder", usbId, channel))
          .Publish(options);
}

ExpansionHubMotor::~ExpansionHubMotor() noexcept {
  m_hub.UnreserveMotor(m_channel);
}

void ExpansionHubMotor::SetPercentagePower(double power) {
  m_modePublisher.Set(kPercentageMode);
  m_setpointPublisher.Set(power);
}

void ExpansionHubMotor::SetVoltage(wpi::units::volt_t voltage) {
  m_modePublisher.Set(kVoltageMode);
  m_setpointPublisher.Set(voltage.to<double>());
}

void ExpansionHubMotor::SetPositionSetpoint(double setpoint) {
  m_modePublisher.Set(kPositionMode);
  m_setpointPublisher.Set(setpoint);
}

void ExpansionHubMotor::SetVelocitySetpoint(double setpoint) {
  m_modePublisher.Set(kVelocityMode);
  m_setpointPublisher.Set(setpoint);
}

void ExpansionHubMotor::SetEnabled(bool enabled) {
  m_enabledPublisher.Set(enabled);
}

void ExpansionHubMotor::SetFloatOn0(bool floatOn0) {
  m_floatOn0Publisher.Set(floatOn0);
}

wpi::units::ampere_t ExpansionHubMotor::GetCurrent() const {
  return wpi::units::ampere_t{m_currentSubscriber.Get(0)};
}

void ExpansionHubMotor::SetDistancePerCount(double perCount) {
  m_distancePerCountPublisher.Set(perCount);
}

double ExpansionHubMotor::GetEncoderVelocity() const {
  return m_encoderVelocitySubscriber.Get(0);
}

double ExpansionHubMotor::GetEncoderPosition() const {
  return m_encoderSubscriber.Get(0);
}

void ExpansionHubMotor::SetReversed(bool reversed) {
  m_reversedPublisher.Set(true);
}

void ExpansionHubMotor::ResetEncoder() {
  m_resetEncoderPublisher.Set(true);
}

ExpansionHubPidConstants& ExpansionHubMotor::GetVelocityPidConstants() {
  return m_velocityPidConstants;
}

ExpansionHubPidConstants& ExpansionHubMotor::GetPositionPidConstants() {
  return m_positionPidConstants;
}

void ExpansionHubMotor::Follow(const ExpansionHubMotor& leader) {
  if (m_hub.GetUsbId() != leader.m_hub.GetUsbId()) {
    throw FRC_MakeError(err::InvalidParameter,
                        "Cannot follow motor on different hub");
  }
  m_modePublisher.Set(kFollowerMode);
  m_setpointPublisher.Set(leader.m_channel);
}
