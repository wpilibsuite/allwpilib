
#include "components/RobotComponent.h"

#include <gazebo/physics/Joint.hh>

#ifndef _COMPONENTS_PISTON_H_
#define _COMPONENTS_PISTON_H_

// PSI in Newtons/m^2
#define PSI(x) (x*6895)

/**
 * Piston models a pneumatic piston. In the URDF/SDF, the joint should
 * be modeled as a prismatic joint with the limits corresponding to
 * extended and retracted positions. Pistons have three states:
 * forward, reverse and off. Currently, the forward and reverse forces
 * are specified in Newtons and are constant. Changes in pressure are
 * not modeled.
 */
class Piston : public RobotComponent {
public:
  Piston(std::string topic, physics::JointPtr joint, double forward_force, double reverse_force);
  virtual ~Piston();

  void connect(transport::NodePtr node);
  void update(bool enabled);
private:
  std::string topic;
  double forward_force, reverse_force, signal;
  transport::SubscriberPtr sub;
  physics::JointPtr joint;

  void callback(const msgs::ConstFloat64Ptr &msg);
};

#endif /* _COMPONENTS_PISTON_H_ */
