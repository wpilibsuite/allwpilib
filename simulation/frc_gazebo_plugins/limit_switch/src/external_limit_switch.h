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

#include <boost/pointer_cast.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/sensors/sensors.hh>

using namespace gazebo;

class ExternalLimitSwitch : public Switch {
 public:
  explicit ExternalLimitSwitch(sdf::ElementPtr sdf);

  /// \brief Returns true when the switch is triggered.
  virtual bool Get();

 private:
  sensors::ContactSensorPtr sensor;
};
