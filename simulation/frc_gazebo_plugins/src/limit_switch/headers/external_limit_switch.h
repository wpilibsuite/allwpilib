// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <boost/pointer_cast.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/sensors/sensors.hh>

#ifdef _WIN32
#include <Winsock2.h>
#endif

#include "switch.h"

class ExternalLimitSwitch : public Switch {
 public:
  explicit ExternalLimitSwitch(sdf::ElementPtr sdf);

  /// \brief Returns true when the switch is triggered.
  bool Get() override;

 private:
  gazebo::sensors::ContactSensorPtr sensor;
};
