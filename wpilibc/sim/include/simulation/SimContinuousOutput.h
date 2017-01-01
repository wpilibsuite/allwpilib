/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <gazebo/transport/transport.hh>

#include "SpeedController.h"

#ifdef _WIN32
// Ensure that Winsock2.h is included before Windows.h, which can get
// pulled in by anybody (e.g., Boost).
#include <Winsock2.h>
#endif

namespace frc {

class SimContinuousOutput {
 private:
  gazebo::transport::PublisherPtr pub;
  double speed;

 public:
  explicit SimContinuousOutput(std::string topic);

  /**
   * Set the output value.
   *
   * The value is set using a range of -1.0 to 1.0, appropriately
   * scaling the value.
   *
   * @param value The value between -1.0 and 1.0 to set.
   */
  void Set(double value);

  /**
   * @return The most recently set value.
   */
  double Get();
};

}  // namespace frc
