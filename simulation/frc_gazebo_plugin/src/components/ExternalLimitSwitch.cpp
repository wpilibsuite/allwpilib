
#include "components/ExternalLimitSwitch.h"

ExternalLimitSwitch::ExternalLimitSwitch(std::string topic, sensors::ContactSensorPtr sensor) {
  std::cout << "Initializing external limit switch: " << topic << std::endl;
  this->topic = topic;
  this->sensor = sensor;
}

ExternalLimitSwitch::~ExternalLimitSwitch() {

}

void ExternalLimitSwitch::connect(transport::NodePtr node) {
  pub = node->Advertise<msgs::Bool>(topic);
}

void ExternalLimitSwitch::update(bool enabled) {
  msg.set_data(sensor->GetContacts().contact().size() > 0);
  pub->Publish(msg);
}
