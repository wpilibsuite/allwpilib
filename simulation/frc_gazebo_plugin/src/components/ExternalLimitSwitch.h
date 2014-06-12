
#include "components/RobotComponent.h"
#include "msgs/bool.pb.h"

#include <gazebo/sensors/sensors.hh>

#ifndef _COMPONENTS_EXTERNAL_LIMIT_SWITCH_H_
#define _COMPONENTS_EXTERNAL_LIMIT_SWITCH_H_

class ExternalLimitSwitch : public RobotComponent {
public:
  ExternalLimitSwitch(std::string topic, sensors::ContactSensorPtr sensor);
  virtual ~ExternalLimitSwitch();

  void connect(transport::NodePtr node);
  void update(bool enabled);
  
private:
  std::string topic;
  transport::PublisherPtr pub;
  msgs::Bool msg;
  sensors::ContactSensorPtr sensor;
};

#endif /* _COMPONENTS_EXTERNAL_LIMIT_SWITCH_H_ */
