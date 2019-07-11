/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
