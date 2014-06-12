
#include "components/Motor.h"

Motor::Motor(std::string topic, physics::JointPtr joint, double multiplier) {
  std::cout << "Initializing motor: " << topic << " -- " << multiplier << std::endl;
  this->topic = topic;
  this->multiplier = multiplier;
  this->joint = joint;
  signal = 0;
}

Motor::~Motor() {

}

void Motor::connect(transport::NodePtr node) {
  sub = node->Subscribe(topic, &Motor::callback, this);
}

void Motor::callback(const msgs::ConstFloat64Ptr &msg) {
  signal = msg->data();
  if (signal < -1) { signal = -1; }
  else if (signal > 1) { signal = 1; }
}

void Motor::update(bool enabled) {
  if (enabled) {
    joint->SetForce(0, signal*multiplier);
  } else {
    joint->SetForce(0, 0);
  }
}
