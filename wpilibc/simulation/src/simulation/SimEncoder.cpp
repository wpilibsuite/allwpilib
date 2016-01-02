/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "simulation/SimEncoder.h"
#include "simulation/MainNode.h"

SimEncoder::SimEncoder(std::string topic) {
	commandPub = MainNode::Advertise<msgs::GzString>("~/simulator/"+topic+"/control");

	posSub = MainNode::Subscribe("~/simulator/"+topic+"/position",
		                 &SimEncoder::positionCallback, this);
	velSub = MainNode::Subscribe("~/simulator/"+topic+"/velocity",
		                 &SimEncoder::velocityCallback, this);

	if (commandPub->WaitForConnection(gazebo::common::Time(5.0))) { // Wait up to five seconds.
		std::cout << "Initialized ~/simulator/" + topic << std::endl;
	} else {
		std::cerr << "Failed to initialize ~/simulator/" + topic + ": does the encoder exist?" << std::endl;
	}
}

void SimEncoder::Reset() {
	sendCommand("reset");
}

void SimEncoder::Start() {
	sendCommand("start");
}

void SimEncoder::Stop() {
	sendCommand("stop");
}

double SimEncoder::GetPosition() {
	return position;
}

double SimEncoder::GetVelocity() {
	return velocity;
}


void SimEncoder::sendCommand(std::string cmd) {
	msgs::GzString msg;
	msg.set_data(cmd);
	commandPub->Publish(msg);
}


void SimEncoder::positionCallback(const msgs::ConstFloat64Ptr &msg) {
	position = msg->data();
}

void SimEncoder::velocityCallback(const msgs::ConstFloat64Ptr &msg) {
	velocity = msg->data();
}
