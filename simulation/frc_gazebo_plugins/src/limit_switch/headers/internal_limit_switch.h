/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
  virtual bool Get();

 private:
  gazebo::physics::JointPtr joint;
  double min, max;
  bool radians;
};
