
#include "components/RobotComponent.h"
#include "msgs/bool.pb.h"

#include <gazebo/sensors/sensors.hh>

#ifndef _COMPONENTS_INTERNAL_LIMIT_SWITCH_H_
#define _COMPONENTS_INTERNAL_LIMIT_SWITCH_H_

class InternalLimitSwitch : public RobotComponent {
public:
  InternalLimitSwitch(std::string topic, physics::JointPtr joint,
                      double min, double max, bool radians);
  virtual ~InternalLimitSwitch();

  void connect(transport::NodePtr node);
  void update(bool enabled);
  
private:
  std::string topic;
  transport::PublisherPtr pub;
  msgs::Bool msg;
  physics::JointPtr joint;
  double min, max;
  bool radians;
};

#endif /* _COMPONENTS_EXTERNAL_LIMIT_SWITCH_H_ */
