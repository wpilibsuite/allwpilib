/*
 * SimSpeedController.cpp
 *
 *  Created on: May 28, 2014
 *      Author: alex
 */

#include "simulation/SimSpeedController.h"
#include "simulation/MainNode.h"

SimSpeedController::SimSpeedController(std::string topic) {
    pub = MainNode::Advertise<msgs::Float64>("~/simulator/"+topic);
	std::cout << "Initialized ~/simulator/"+topic << std::endl;
}

void SimSpeedController::Set(float speed, uint8_t syncGroup) {
	Set(speed);
}

void SimSpeedController::Set(float speed) {
	msgs::Float64 msg;
	msg.set_data(speed);
	pub->Publish(msg);
}

float SimSpeedController::Get() {
	return speed;
}

void SimSpeedController::Disable() {
	Set(0);
}

void SimSpeedController::PIDWrite(float output) {
	Set(output);
}



