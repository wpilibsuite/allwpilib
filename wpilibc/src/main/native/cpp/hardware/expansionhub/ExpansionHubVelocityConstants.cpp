// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/expansionhub/ExpansionHubVelocityConstants.hpp"

#include "fmt/core.h"
#include "wpi/system/SystemServer.hpp"

using namespace wpi;

ExpansionHubVelocityConstants::ExpansionHubVelocityConstants(int hubNumber,
                                                             int motorNumber) {
  auto systemServer = SystemServer::GetSystemServer();

  wpi::nt::PubSubOptions options;
  options.sendAll = true;
  options.keepDuplicates = true;
  options.periodic = 0.005;

  m_pPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/velocity/kp",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_iPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/velocity/ki",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_dPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/velocity/kd",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_sPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/velocity/ks",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_vPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/velocity/kv",
                                      hubNumber, motorNumber))
          .Publish(options);

  m_aPublisher =
      systemServer
          .GetDoubleTopic(fmt::format("/rhsp/{}/motor{}/constants/velocity/ka",
                                      hubNumber, motorNumber))
          .Publish(options);
}

ExpansionHubVelocityConstants& ExpansionHubVelocityConstants::SetPID(double p,
                                                                     double i,
                                                                     double d) {
  m_pPublisher.Set(p);
  m_iPublisher.Set(i);
  m_dPublisher.Set(d);
  return *this;
}

ExpansionHubVelocityConstants& ExpansionHubVelocityConstants::SetFF(double s,
                                                                    double v,
                                                                    double a) {
  m_sPublisher.Set(s);
  m_vPublisher.Set(v);
  m_aPublisher.Set(a);
  return *this;
}
