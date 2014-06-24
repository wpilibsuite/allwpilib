
#include "components/Piston.h"

Piston::Piston(std::string topic, physics::JointPtr joint,
               double forward_force, double reverse_force) {
  std::cout << "Initializing piston: " << topic << " -- " << forward_force
            << "/" << reverse_force << "N"  << std::endl;
  this->topic = topic;
  this->forward_force = forward_force;
  this->reverse_force = reverse_force;
  this->joint = joint;
  signal = -1;
}

Piston::~Piston() {

}

void Piston::connect(transport::NodePtr node) {
  sub = node->Subscribe(topic, &Piston::callback, this);
}

void Piston::callback(const msgs::ConstFloat64Ptr &msg) {
  signal = msg->data();
  if (signal < -0.001) { signal = -reverse_force; }
  else if (signal > 0.001) { signal = forward_force; }
  else { signal = 0; }
}

void Piston::update(bool enabled) {
  joint->SetForce(0, signal);
}
