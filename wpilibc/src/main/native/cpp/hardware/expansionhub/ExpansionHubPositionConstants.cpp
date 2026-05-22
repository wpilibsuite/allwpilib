// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/expansionhub/ExpansionHubPositionConstants.hpp"

#include "fmt/core.h"
#include "wpi/system/SystemServer.hpp"

using namespace wpi;

ExpansionHubPositionConstants::ExpansionHubPositionConstants(int hubNumber,
                                                             int motorNumber) {
  auto systemServer = SystemServer::GetSystemServer();

  wpi::nt::PubSubOptions options;
  options.sendAll = true;
  options.keepDuplicates = true;
  options.periodic = 0.005;

  m_pPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/position/kp",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_iPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/position/ki",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_dPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/position/kd",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_sPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/position/ks",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_continuousPublisher =
      systemServer
          .GetBooleanTopic(
              fmt::format("/rhsp/{}/motor{}/constants/position/continuous",
                          hubNumber, motorNumber))
          .Publish(options);

  m_continuousMinimumPublisher =
      systemServer
          .GetDoubleTopic(fmt::format(
              "/rhsp/{}/motor{}/constants/position/continuousMinimum",
              hubNumber, motorNumber))
          .Publish(options);

  m_continuousMaximumPublisher =
      systemServer
          .GetDoubleTopic(fmt::format(
              "/rhsp/{}/motor{}/constants/position/continuousMaximum",
              hubNumber, motorNumber))
          .Publish(options);
}

ExpansionHubPositionConstants& ExpansionHubPositionConstants::SetPID(double p,
                                                                     double i,
                                                                     double d) {
  m_pPublisher.Set(p);
  m_iPublisher.Set(i);
  m_dPublisher.Set(d);
  return *this;
}

ExpansionHubPositionConstants& ExpansionHubPositionConstants::SetS(double s) {
  m_sPublisher.Set(s);
  return *this;
}

ExpansionHubPositionConstants&
ExpansionHubPositionConstants::EnableContinuousInput(double minimumInput,
                                                     double maximumInput) {
  m_continuousMaximumPublisher.Set(maximumInput);
  m_continuousMinimumPublisher.Set(minimumInput);
  m_continuousPublisher.Set(true);
  return *this;
}

ExpansionHubPositionConstants&
ExpansionHubPositionConstants::DisableContinuousInput() {
  m_continuousPublisher.Set(false);
  return *this;
}
