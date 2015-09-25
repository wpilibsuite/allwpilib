/*
 * SimDigitalInput.cpp
 *
 *  Created on: May 28, 2014
 *      Author: alex
 */

#include "simulation/SimDigitalInput.h"
#include "simulation/MainNode.h"

SimDigitalInput::SimDigitalInput(std::string topic) {
    sub = MainNode::Subscribe("~/simulator/"+topic, &SimDigitalInput::callback, this);
	std::cout << "Initialized ~/simulator/"+topic << std::endl;
}

bool SimDigitalInput::Get() {
	return value;
}

void SimDigitalInput::callback(const msgs::ConstBoolPtr &msg) {
  value = msg->data();
}
