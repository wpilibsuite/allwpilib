/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <gazebo/transport/transport.hh>

#include "simulation/gz_msgs/msgs.h"

namespace frc {

class SimFloatInput {
 public:
  explicit SimFloatInput(std::string topic);

  /**
   * @return The value of the potentiometer.
   */
  double Get();

 private:
  double value;
  gazebo::transport::SubscriberPtr sub;
  void callback(const gazebo::msgs::ConstFloat64Ptr& msg);
};

}  // namespace frc
