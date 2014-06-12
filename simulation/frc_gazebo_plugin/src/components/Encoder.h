
#include "components/RobotComponent.h"

#include <gazebo/physics/Joint.hh>

#ifndef _COMPONENTS_ENCODER_H_
#define _COMPONENTS_ENCODER_H_

class Encoder : public RobotComponent {
public:
  Encoder(std::string topic, physics::JointPtr joint);
  virtual ~Encoder();

  void connect(transport::NodePtr node);
  void update(bool enabled);
private:
  std::string topic;
  bool stopped;
  double zero, stop_value;
  transport::SubscriberPtr command_sub;
  transport::PublisherPtr pos_pub, vel_pub;
  msgs::Float64 pos_msg, vel_msg;
  physics::JointPtr joint;

  void callback(const msgs::ConstStringPtr &msg);
};

#endif /* _COMPONENTS_ENCODER_H_ */
