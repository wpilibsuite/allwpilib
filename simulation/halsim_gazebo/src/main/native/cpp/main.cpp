// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/core.h>
#include <hal/Ports.h>

#include "GazeboAnalogIn.h"
#include "GazeboDIO.h"
#include "GazeboEncoder.h"
#include "GazeboPCM.h"
#include "GazeboPWM.h"
#include "HALSimGazebo.h"

/* Currently, robots never terminate, so we keep a single static object
   to access Gazebo with and it is never properly released or cleaned up. */
static HALSimGazebo halsim;

extern "C" {
int HALSIM_InitExtension(void) {
  fmt::print("Gazebo Simulator Initializing.\n");

  if (!halsim.node.Connect()) {
    fmt::print(stderr,
               "Error: unable to connect to Gazebo.  Is it running?.\n");
    return -1;
  }
  fmt::print("Gazebo Simulator Connected.\n");

  for (int i = 0; i < HALSimGazebo::kPWMCount; i++)
    halsim.pwms[i] = new GazeboPWM(i, &halsim);

  for (int i = 0; i < HALSimGazebo::kPCMCount; i++)
    halsim.pcms[i] = new GazeboPCM(0, i, &halsim);
  GazeboPCM_SetPressureSwitch(0, true);

  for (int i = 0; i < HALSimGazebo::kEncoderCount; i++)
    halsim.encoders[i] = new GazeboEncoder(i, &halsim);

  int analog_in_count = HAL_GetNumAnalogInputs();
  for (int i = 0; i < analog_in_count; i++)
    halsim.analog_inputs.push_back(new GazeboAnalogIn(i, &halsim));

  int dio_count = HAL_GetNumDigitalChannels();
  for (int i = 0; i < dio_count; i++)
    halsim.dios.push_back(new GazeboDIO(i, &halsim));

  return 0;
}
}  // extern "C"
