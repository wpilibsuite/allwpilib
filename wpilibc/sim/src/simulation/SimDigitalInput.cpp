/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "simulation/SimDigitalInput.h"

#include "simulation/MainNode.h"

using namespace frc;

SimDigitalInput::SimDigitalInput(std::string topic) {
  sub = MainNode::Subscribe("~/simulator/" + topic, &SimDigitalInput::callback,
                            this);
  std::cout << "Initialized ~/simulator/" + topic << std::endl;
}

bool SimDigitalInput::Get() { return value; }

void SimDigitalInput::callback(const gazebo::msgs::ConstBoolPtr& msg) {
  value = msg->data();
}
