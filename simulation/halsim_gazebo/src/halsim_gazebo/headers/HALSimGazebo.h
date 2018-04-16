/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "GazeboNode.h"

class GazeboPWM;
class GazeboPCM;
class GazeboEncoder;

class HALSimGazebo {
 public:
  static const int kPWMCount = 20;
  static const int kPCMCount = 8;
  static const int kEncoderCount = 8;

  GazeboNode node;
  GazeboPWM* pwms[kPWMCount];
  GazeboPCM* pcms[kPCMCount];
  GazeboEncoder* encoders[kEncoderCount];
};
