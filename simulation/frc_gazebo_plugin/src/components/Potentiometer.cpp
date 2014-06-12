
#include "components/Potentiometer.h"

Potentiometer::Potentiometer(std::string topic, physics::JointPtr joint, bool radians) {
  std::cout << "Initializing potentiometer: " << topic << std::endl;
  this->topic = topic;
  this->joint = joint;
  this->radians = radians;
}

Potentiometer::~Potentiometer() {

}

void Potentiometer::connect(transport::NodePtr node) {
  pub = node->Advertise<msgs::Float64>(topic);
}

void Potentiometer::update(bool enabled) {
  joint->GetAngle(0).Normalize();
  if (radians) {
    msg.set_data(joint->GetAngle(0).Radian());
  } else {
    msg.set_data(joint->GetAngle(0).Degree());
  }
  pub->Publish(msg);
}
