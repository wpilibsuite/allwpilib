/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "simulation/SimGyro.h"

#include "llvm/raw_ostream.h"
#include "simulation/MainNode.h"

frc::SimGyro::SimGyro(std::string topic) {
  commandPub = MainNode::Advertise<gazebo::msgs::GzString>("~/simulator/" +
                                                           topic + "/control");

  posSub = MainNode::Subscribe("~/simulator/" + topic + "/position",
                               &frc::SimGyro::positionCallback, this);
  velSub = MainNode::Subscribe("~/simulator/" + topic + "/velocity",
                               &frc::SimGyro::velocityCallback, this);

  if (commandPub->WaitForConnection(
          gazebo::common::Time(5.0))) {  // Wait up to five seconds.
    llvm::outs() << "Initialized ~/simulator/" + topic << "\n";
  } else {
    llvm::errs() << "Failed to initialize ~/simulator/" + topic +
                        ": does the gyro exist?\n";
  }
}

void frc::SimGyro::Reset() { sendCommand("reset"); }

double frc::SimGyro::GetAngle() { return position; }

double frc::SimGyro::GetVelocity() { return velocity; }

void frc::SimGyro::sendCommand(std::string cmd) {
  gazebo::msgs::GzString msg;
  msg.set_data(cmd);
  commandPub->Publish(msg);
}

void frc::SimGyro::positionCallback(const gazebo::msgs::ConstFloat64Ptr& msg) {
  position = msg->data();
}

void frc::SimGyro::velocityCallback(const gazebo::msgs::ConstFloat64Ptr& msg) {
  velocity = msg->data();
}
