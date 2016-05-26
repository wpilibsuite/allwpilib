/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "simulation/SimContinuousOutput.h"
#include "simulation/MainNode.h"

SimContinuousOutput::SimContinuousOutput(std::string topic) {
  pub = MainNode::Advertise<msgs::Float64>("~/simulator/" + topic);
  std::cout << "Initialized ~/simulator/" + topic << std::endl;
}

void SimContinuousOutput::Set(float speed) {
  msgs::Float64 msg;
  msg.set_data(speed);
  pub->Publish(msg);
}

float SimContinuousOutput::Get() { return speed; }
