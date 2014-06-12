
#include "components/InternalLimitSwitch.h"

InternalLimitSwitch::InternalLimitSwitch(std::string topic, physics::JointPtr joint,
                                         double min, double max, bool radians) {
  std::cout << "Initializing external limit switch: " << topic << std::endl;
  this->topic = topic;
  this->joint = joint;
  this->min = min;
  this->max = max;
  this->radians = radians;
}

InternalLimitSwitch::~InternalLimitSwitch() {

}

void InternalLimitSwitch::connect(transport::NodePtr node) {
  pub = node->Advertise<msgs::Bool>(topic);
}

void InternalLimitSwitch::update(bool enabled) {
  double value;
  joint->GetAngle(0).Normalize();
  if (radians) {
    value = joint->GetAngle(0).Radian();
  } else {
    value = joint->GetAngle(0).Degree();
  }
  msg.set_data(value >= min && value <= max);
  pub->Publish(msg);
}
