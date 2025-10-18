// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ExpansionHubPidConstants.h"

#include "frc/Errors.h"

#include "frc/SystemServer.h"

#include <string>

using namespace frc;

ExpansionHubPidConstants::ExpansionHubPidConstants(int usbId, int channel,
                                                   bool isVelocityPid) {
  auto systemServer = SystemServer::GetSystemServer();

  nt::PubSubOptions options;
  options.sendAll = true;
  options.keepDuplicates = true;
  options.periodic = 0.005;

  std::string pidType = isVelocityPid ? "velocity" : "position";

  m_pPublisher = systemServer
                     .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/pid/{}/kp",
                                                 usbId, channel, pidType))
                     .Publish(options);

  m_iPublisher = systemServer
                     .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/pid/{}/ki",
                                                 usbId, channel, pidType))
                     .Publish(options);

  m_dPublisher = systemServer
                     .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/pid/{}/kd",
                                                 usbId, channel, pidType))
                     .Publish(options);

  m_aPublisher = systemServer
                     .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/pid/{}/ka",
                                                 usbId, channel, pidType))
                     .Publish(options);

  m_vPublisher = systemServer
                     .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/pid/{}/kv",
                                                 usbId, channel, pidType))
                     .Publish(options);

  m_sPublisher = systemServer
                     .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/pid/{}/ks",
                                                 usbId, channel, pidType))
                     .Publish(options);

  m_continuousPublisher =
      systemServer
          .GetBooleanTopic(fmt::format("/rhsp/{}/motor{}/pid/{}/continous",
                                       usbId, channel, pidType))
          .Publish(options);

  m_continuousMinimumPublisher =
      systemServer
          .GetDoubleTopic(
              fmt::format("/rhsp/{}/motor{}/pid/{}/continuousMinimum", usbId,
                          channel, pidType))
          .Publish(options);

  m_continuousMaximumPublisher =
      systemServer
          .GetDoubleTopic(
              fmt::format("/rhsp/{}/motor{}/pid/{}/continousMaximum", usbId,
                          channel, pidType))
          .Publish(options);
}

void ExpansionHubPidConstants::SetPID(double p, double i, double d) {
  m_pPublisher.Set(p);
  m_iPublisher.Set(i);
  m_dPublisher.Set(d);
}

void ExpansionHubPidConstants::SetFF(double s, double v, double a) {
  m_sPublisher.Set(s);
  m_vPublisher.Set(v);
  m_aPublisher.Set(a);
}

void ExpansionHubPidConstants::EnableContinousInput(double minimumInput,
                                                    double maximumInput) {
  m_continuousMaximumPublisher.Set(maximumInput);
  m_continuousMinimumPublisher.Set(minimumInput);
  m_continuousPublisher.Set(true);
}

void ExpansionHubPidConstants::DisableContinousInput() {
  m_continuousPublisher.Set(false);
}
