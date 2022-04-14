// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include "GazeboNode.h"

class GazeboPWM;
class GazeboPCM;
class GazeboEncoder;
class GazeboAnalogIn;
class GazeboDIO;

class HALSimGazebo {
 public:
  static const int kPWMCount = 20;
  static const int kPCMCount = 8;
  static const int kEncoderCount = 8;

  GazeboNode node;
  GazeboPWM* pwms[kPWMCount];
  GazeboPCM* pcms[kPCMCount];
  GazeboEncoder* encoders[kEncoderCount];
  std::vector<GazeboAnalogIn*> analog_inputs;
  std::vector<GazeboDIO*> dios;
};
