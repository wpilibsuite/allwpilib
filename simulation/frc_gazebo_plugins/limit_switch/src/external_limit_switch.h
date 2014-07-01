#pragma once

#include <gazebo/gazebo.hh>

#include "switch.h"

using namespace gazebo;

class ExternalLimitSwitch : public Switch {
public: 
  ExternalLimitSwitch(sdf::ElementPtr sdf);
  ~ExternalLimitSwitch();
  
  /// \brief Returns true when the switch is triggered.
  virtual bool Get();

private:
  sensors::ContactSensorPtr sensor;
};
