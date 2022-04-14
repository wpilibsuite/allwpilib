// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

#ifdef _WIN32
#include <Winsock2.h>
#endif

#include "switch.h"

class InternalLimitSwitch : public Switch {
 public:
  InternalLimitSwitch(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Returns true when the switch is triggered.
  bool Get() override;

 private:
  gazebo::physics::JointPtr joint;
  double min, max;
  bool radians;
};
