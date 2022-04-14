// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "HALSimGazebo.h"

class GazeboPWM {
 public:
  GazeboPWM(int port, HALSimGazebo* halsim);
  void SetInitialized(bool value) { m_initialized = value; }
  bool IsInitialized(void) { return m_initialized; }
  void Publish(double value);

 private:
  HALSimGazebo* m_halsim;
  gazebo::transport::PublisherPtr m_pub;
  bool m_initialized;
  int m_port;
};
