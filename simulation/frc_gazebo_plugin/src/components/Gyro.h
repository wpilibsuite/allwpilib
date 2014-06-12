
#include "components/RobotComponent.h"

#include <gazebo/physics/Link.hh>

#ifndef _COMPONENTS_GYRO_H_
#define _COMPONENTS_GYRO_H_

typedef enum {Roll, Pitch, Yaw} ROTATION;

class Gyro : public RobotComponent {
public:
  Gyro(std::string topic, physics::LinkPtr link, ROTATION axis);
  virtual ~Gyro();

  void connect(transport::NodePtr node);
  void update(bool enabled);
private:
  ROTATION axis;
  std::string topic;
  double zero;
  transport::SubscriberPtr command_sub;
  transport::PublisherPtr pos_pub, vel_pub;
  msgs::Float64 pos_msg, vel_msg;
  physics::LinkPtr link;

  void callback(const msgs::ConstStringPtr &msg);
};

#endif /* _COMPONENTS_GYRO_H_ */
