/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <gazebo/common/Time.hh>
#include <gazebo/transport/transport.hh>

#include "simulation/gz_msgs/msgs.h"

namespace frc {

class SimEncoder {
 public:
  explicit SimEncoder(std::string topic);

  void Reset();
  void Start();
  void Stop();
  double GetPosition();
  double GetVelocity();

 private:
  void sendCommand(std::string cmd);

  double position, velocity;
  gazebo::transport::SubscriberPtr posSub, velSub;
  gazebo::transport::PublisherPtr commandPub;
  void positionCallback(const gazebo::msgs::ConstFloat64Ptr& msg);
  void velocityCallback(const gazebo::msgs::ConstFloat64Ptr& msg);
};

}  // namespace frc
