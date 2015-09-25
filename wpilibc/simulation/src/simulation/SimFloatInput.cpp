/*
 * SimFloatInput.cpp
 *
 *  Created on: May 28, 2014
 *      Author: alex
 */

#include "simulation/SimFloatInput.h"
#include "simulation/MainNode.h"

SimFloatInput::SimFloatInput(std::string topic) {
    sub = MainNode::Subscribe("~/simulator/"+topic, &SimFloatInput::callback, this);
	std::cout << "Initialized ~/simulator/"+topic << std::endl;
}

double SimFloatInput::Get() {
	return value;
}

void SimFloatInput::callback(const msgs::ConstFloat64Ptr &msg) {
  value = msg->data();
}
