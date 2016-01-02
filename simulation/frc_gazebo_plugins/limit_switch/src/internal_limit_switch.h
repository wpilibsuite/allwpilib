/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "switch.h"

#ifdef _WIN32
	#include <Winsock2.h>
#endif
#include <gazebo/physics/physics.hh>
#include <gazebo/gazebo.hh>

using namespace gazebo;

class InternalLimitSwitch : public Switch {
public:
  InternalLimitSwitch(physics::ModelPtr model, sdf::ElementPtr sdf);
  ~InternalLimitSwitch();

  /// \brief Returns true when the switch is triggered.
  virtual bool Get();

private:
  physics::JointPtr joint;
  double min, max;
  bool radians;
};
