#pragma once

#include "switch.h"

#ifdef _WIN32
	#include <Winsock2.h>
#endif
#include <gazebo/physics/physics.hh>
#include <gazebo/gazebo.hh>

using namespace gazebo;

class InternalLimitSwitch : public Switch {
public:
  InternalLimitSwitch(physics::ModelPtr model, sdf::ElementPtr sdf);
  ~InternalLimitSwitch();

  /// \brief Returns true when the switch is triggered.
  virtual bool Get();

private:
  physics::JointPtr joint;
  double min, max;
  bool radians;
};
