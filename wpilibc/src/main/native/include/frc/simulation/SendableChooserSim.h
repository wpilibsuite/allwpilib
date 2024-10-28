// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>

#include "frc/RobotBase.h"

namespace frc::sim {

/**
 * Class that facilitates control of a SendableChooser's selected option in
 * simulation.
 */
class SendableChooserSim {
 public:
  /**
   * Constructs a SendableChooserSim.
   *
   * @param path The path where the SendableChooser is published.
   */
  explicit SendableChooserSim(std::string_view path);

  /**
   * Constructs a SendableChooserSim.
   *
   * @param inst The NetworkTables instance.
   * @param path The path where the SendableChooser is published.
   */
  SendableChooserSim(nt::NetworkTableInstance inst, std::string_view path);

  /**
   * Set the selected option.
   * @param option The option.
   */
  void SetSelected(std::string_view option);

 private:
  nt::StringPublisher m_publisher;
};
}  // namespace frc::sim
