/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "simulation/SimFloatInput.h"

#include "simulation/MainNode.h"

using namespace frc;

SimFloatInput::SimFloatInput(std::string topic) {
  sub = MainNode::Subscribe("~/simulator/" + topic, &SimFloatInput::callback,
                            this);
  std::cout << "Initialized ~/simulator/" + topic << std::endl;
}

double SimFloatInput::Get() { return value; }

void SimFloatInput::callback(const gazebo::msgs::ConstFloat64Ptr& msg) {
  value = msg->data();
}
