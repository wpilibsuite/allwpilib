// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/SendableChooserSim.h"

using namespace frc::sim;

SendableChooserSim::SendableChooserSim(std::string_view path)
    : SendableChooserSim(nt::NetworkTableInstance::GetDefault(), path) {}

SendableChooserSim::SendableChooserSim(nt::NetworkTableInstance inst,
                                       std::string_view path) {
  if constexpr (RobotBase::IsSimulation()) {
    m_publisher =
        inst.GetStringTopic(fmt::format("{}{}", path, "selected")).Publish();
  }
}

void SendableChooserSim::SetSelected(std::string_view option) {
  if constexpr (RobotBase::IsSimulation()) {
    m_publisher.Set(option);
  }
}
