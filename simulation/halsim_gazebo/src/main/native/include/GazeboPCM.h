/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HALSimGazebo.h"

class GazeboPCM {
 public:
  GazeboPCM(int index, int channel, HALSimGazebo* halsim);
  void Publish(bool value);
  void SetInitialized(bool value) { m_initialized = value; }
  bool IsInitialized(void) { return m_initialized; }

 private:
  HALSimGazebo* m_halsim;
  int m_index;
  int m_channel;

  bool m_initialized;

  gazebo::transport::PublisherPtr m_pub;
};

void GazeboPCM_SetPressureSwitch(int index, bool value);
