/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "GazeboEncoder.h"
#include "GazeboPCM.h"
#include "GazeboPWM.h"
#include "HALSimGazebo.h"

/* Currently, robots never terminate, so we keep a single static object
   to access Gazebo with and it is never properly released or cleaned up. */
static HALSimGazebo halsim;

extern "C" {
int HALSIM_InitExtension(void) {
  std::cout << "Gazebo Simulator Initializing." << std::endl;

  if (!halsim.node.Connect()) {
    std::cerr << "Error: unable to connect to Gazebo.  Is it running?."
              << std::endl;
    return -1;
  }
  std::cout << "Gazebo Simulator Connected." << std::endl;

  for (int i = 0; i < HALSimGazebo::kPWMCount; i++)
    halsim.pwms[i] = new GazeboPWM(i, &halsim);

  for (int i = 0; i < HALSimGazebo::kPCMCount; i++)
    halsim.pcms[i] = new GazeboPCM(0, i, &halsim);
  GazeboPCM_SetPressureSwitch(0, true);

  for (int i = 0; i < HALSimGazebo::kEncoderCount; i++)
    halsim.encoders[i] = new GazeboEncoder(i, &halsim);

  return 0;
}
}  // extern "C"
