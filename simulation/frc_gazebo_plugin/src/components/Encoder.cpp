
#include "components/Encoder.h"
#define _USE_MATH_DEFINES
#include <math.h>

Encoder::Encoder(std::string topic, physics::JointPtr joint) {
  std::cout << "Initializing encoder: " << topic << std::endl;
  this->topic = topic;
  this->joint = joint;
}

Encoder::~Encoder() {

}

void Encoder::connect(transport::NodePtr node) {
  command_sub = node->Subscribe(topic+"/control", &Encoder::callback, this);
  pos_pub = node->Advertise<msgs::Float64>(topic+"/position");
  vel_pub = node->Advertise<msgs::Float64>(topic+"/velocity");
  zero = joint->GetAngle(0).Degree();
  stopped = true;
  stop_value = 0;
}

void Encoder::callback(const msgs::ConstStringPtr &msg) {
  std::string command = msg->data();
  if (command == "reset") {
    zero = joint->GetAngle(0).Degree();
  } else if (command == "start") {
    stopped = false;
    zero = (joint->GetAngle(0).Degree() - stop_value);
  } else if  (command == "stop") {
    stopped = true;
    stop_value = joint->GetAngle(0).Degree();
  } else {
    std::cout << "WARNING: Encoder got unknown command '" << command << "'." << std::endl;
  }
}

void Encoder::update(bool enabled) {
  if (stopped) {
    pos_msg.set_data(stop_value);
    pos_pub->Publish(pos_msg);
    vel_msg.set_data(0);
    vel_pub->Publish(vel_msg);
  } else {
    pos_msg.set_data(joint->GetAngle(0).Degree() - zero);
    pos_pub->Publish(pos_msg);
    vel_msg.set_data(joint->GetVelocity(0) * (180.0 / M_PI));
    vel_pub->Publish(vel_msg);
  }
}
