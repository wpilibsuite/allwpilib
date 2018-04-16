/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HALSimGazebo.h"

class GazeboPWM {
 public:
  GazeboPWM(int port, HALSimGazebo* halsim);
  void Publish(double value);

 private:
  HALSimGazebo* m_halsim;
  gazebo::transport::PublisherPtr m_pub;
  int m_port;
};
