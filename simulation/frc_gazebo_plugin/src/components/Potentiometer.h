
#include "components/RobotComponent.h"
#include "msgs/float64.pb.h"

#include <gazebo/physics/Joint.hh>

#ifndef _COMPONENTS_POTENTIOMETER_H_
#define _COMPONENTS_POTENTIOMETER_H_

class Potentiometer : public RobotComponent {
public:
  Potentiometer(std::string topic, physics::JointPtr joint, bool radians);
  virtual ~Potentiometer();

  void connect(transport::NodePtr node);
  void update(bool enabled);
  
private:
  std::string topic;
  transport::PublisherPtr pub;
  msgs::Float64 msg;
  physics::JointPtr joint;
  bool radians;
};

#endif /* _COMPONENTS_POTENTIOMETER_H_ */
