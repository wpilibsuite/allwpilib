
#include "components/RobotComponent.h"

#include <gazebo/physics/Joint.hh>

#ifndef _COMPONENTS_MOTOR_H_
#define _COMPONENTS_MOTOR_H_

class Motor : public RobotComponent {
public:
  Motor(std::string topic, physics::JointPtr joint, double multiplier);
  virtual ~Motor();

  void connect(transport::NodePtr node);
  void update(bool enabled);
private:
  std::string topic;
  double signal, multiplier;
  transport::SubscriberPtr sub;
  physics::JointPtr joint;

  void callback(const msgs::ConstFloat64Ptr &msg);
};

#endif /* _COMPONENTS_MOTOR_H_ */
