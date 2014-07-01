#pragma once

#include <gazebo/gazebo.hh>

#include "switch.h"

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
