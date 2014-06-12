
#include "components/RobotComponent.h"
#include "msgs/float64.pb.h"

#include <gazebo/sensors/sensors.hh>

#ifndef _COMPONENTS_RANGEFINDER_H_
#define _COMPONENTS_RANGEFINDER_H_

class Rangefinder : public RobotComponent {
public:
  Rangefinder(std::string topic, sensors::SonarSensorPtr sensor);
  virtual ~Rangefinder();

  void connect(transport::NodePtr node);
  void update(bool enabled);
  
private:
  std::string topic;
  transport::PublisherPtr pub;
  msgs::Float64 msg;
  physics::LinkPtr link;
  sensors::SonarSensorPtr sensor;
};

#endif /* _COMPONENTS_RANGEFINDER_H_ */
