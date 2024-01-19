// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>

#include "frc/RobotBase.h"

namespace frc::sim {

class SendableChooserSim {
 public:
  explicit SendableChooserSim(std::string_view path);
  SendableChooserSim(nt::NetworkTableInstance inst, std::string_view path);
  void SetSelected(std::string_view option);

 private:
  nt::StringPublisher m_publisher;
};
}  // namespace frc::sim
