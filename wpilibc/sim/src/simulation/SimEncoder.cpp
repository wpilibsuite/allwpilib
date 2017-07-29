/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "simulation/SimEncoder.h"

#include "llvm/raw_ostream.h"
#include "simulation/MainNode.h"

frc::SimEncoder::SimEncoder(std::string topic) {
  commandPub = MainNode::Advertise<gazebo::msgs::GzString>("~/simulator/" +
                                                           topic + "/control");

  posSub = MainNode::Subscribe("~/simulator/" + topic + "/position",
                               &frc::SimEncoder::positionCallback, this);
  velSub = MainNode::Subscribe("~/simulator/" + topic + "/velocity",
                               &frc::SimEncoder::velocityCallback, this);

  if (commandPub->WaitForConnection(
          gazebo::common::Time(5.0))) {  // Wait up to five seconds.
    llvm::outs() << "Initialized ~/simulator/" + topic << "\n";
  } else {
    llvm::errs() << "Failed to initialize ~/simulator/" + topic +
                        ": does the encoder exist?\n";
  }
}

void frc::SimEncoder::Reset() { sendCommand("reset"); }

void frc::SimEncoder::Start() { sendCommand("start"); }

void frc::SimEncoder::Stop() { sendCommand("stop"); }

double frc::SimEncoder::GetPosition() { return position; }

double frc::SimEncoder::GetVelocity() { return velocity; }

void frc::SimEncoder::sendCommand(std::string cmd) {
  gazebo::msgs::GzString msg;
  msg.set_data(cmd);
  commandPub->Publish(msg);
}

void frc::SimEncoder::positionCallback(
    const gazebo::msgs::ConstFloat64Ptr& msg) {
  position = msg->data();
}

void frc::SimEncoder::velocityCallback(
    const gazebo::msgs::ConstFloat64Ptr& msg) {
  velocity = msg->data();
}
