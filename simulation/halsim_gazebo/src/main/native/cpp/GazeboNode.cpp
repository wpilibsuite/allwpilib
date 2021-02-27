// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "GazeboNode.h"

bool GazeboNode::Connect() {
  bool success = gazebo::client::setup();

  if (success) {
    main = gazebo::transport::NodePtr(new gazebo::transport::Node());
    main->Init("frc");
    gazebo::transport::run();
  }

  return success;
}
