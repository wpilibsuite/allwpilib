/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "GazeboNode.h"

class GazeboPWM;

class HALSimGazebo {
 public:
  static const int kPWMCount = 20;

  GazeboNode node;
  GazeboPWM* pwms[kPWMCount];
};
