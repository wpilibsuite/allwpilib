
#include "components/Rangefinder.h"

Rangefinder::Rangefinder(std::string topic, sensors::SonarSensorPtr sensor) {
  std::cout << "Initializing rangefinder: " << topic << std::endl;
  this->topic = topic;
  this->sensor = sensor;
}

Rangefinder::~Rangefinder() {

}

void Rangefinder::connect(transport::NodePtr node) {
  pub = node->Advertise<msgs::Float64>(topic);
}

void Rangefinder::update(bool enabled) {
  msg.set_data(sensor->GetRange());
  pub->Publish(msg);
}
