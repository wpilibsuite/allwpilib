/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "simulation/SimContinuousOutput.h"

#include "simulation/MainNode.h"

using namespace frc;

SimContinuousOutput::SimContinuousOutput(std::string topic) {
  pub = MainNode::Advertise<gazebo::msgs::Float64>("~/simulator/" + topic);
  std::cout << "Initialized ~/simulator/" + topic << std::endl;
}

void SimContinuousOutput::Set(double speed) {
  gazebo::msgs::Float64 msg;
  msg.set_data(speed);
  pub->Publish(msg);
}

double SimContinuousOutput::Get() { return speed; }
