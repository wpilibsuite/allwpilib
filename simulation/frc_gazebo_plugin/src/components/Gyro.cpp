
#include "components/Gyro.h"

#include <gazebo/math/gzmath.hh>
#define _USE_MATH_DEFINES
#include <math.h>

Gyro::Gyro(std::string topic, physics::LinkPtr link, ROTATION axis) {
  std::cout << "Initializing gyro: " << topic << " axis=" << axis << std::endl;
  this->topic = topic;
  this->link = link;
  this->axis = axis;
}

Gyro::~Gyro() {

}

void Gyro::connect(transport::NodePtr node) {
  command_sub = node->Subscribe(topic+"/control", &Gyro::callback, this);
  pos_pub = node->Advertise<msgs::Float64>(topic+"/position");
  vel_pub = node->Advertise<msgs::Float64>(topic+"/velocity");
}

void Gyro::callback(const msgs::ConstStringPtr &msg) {
  std::string command = msg->data();
  if (command == "reset") {
    zero = link->GetWorldCoGPose().rot.GetAsEuler()[axis];
  } else {
    std::cout << "WARNING: Encoder got unknown command '" << command << "'." << std::endl;
  }
}

void Gyro::update(bool enabled) {
  math::Vector3 rot;
  pos_msg.set_data((link->GetWorldCoGPose().rot.GetAsEuler()[axis] - zero)  * (180.0 / M_PI));
  if (pos_msg.data() < -180) pos_msg.set_data(pos_msg.data() + 360);
  else if (pos_msg.data() > 180) pos_msg.set_data(pos_msg.data() - 360);
  pos_pub->Publish(pos_msg);
  vel_msg.set_data(link->GetRelativeAngularVel()[axis] * (180.0 / M_PI));
  vel_pub->Publish(vel_msg);
}
