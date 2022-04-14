// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "HALSimGazebo.h"
#include "simulation/gz_msgs/msgs.h"

class GazeboDIO {
 public:
  GazeboDIO(int index, HALSimGazebo* halsim);
  void SetInitialized(bool value) { m_initialized = value; }
  bool IsInitialized(void) { return m_initialized; }
  void Listen(void);

 private:
  HALSimGazebo* m_halsim;
  int m_index;
  bool m_initialized;
  void Callback(const gazebo::msgs::ConstBoolPtr& msg);
  gazebo::transport::SubscriberPtr m_sub;
};
