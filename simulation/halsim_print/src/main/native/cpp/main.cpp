/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <hal/Ports.h>

#include "HALSimPrint.h"
#include "PrintPWM.h"

// Currently, robots never terminate, so we keep a single static object and it
// is never properly released or cleaned up.
static HALSimPrint halsim;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  std::cout << "Print Simulator Initializing." << std::endl;

  int pwmCount = HAL_GetNumPWMChannels();
  halsim.m_pwms.reserve(pwmCount);
  for (int i = 0; i < pwmCount; i++) halsim.m_pwms.emplace_back(i);

  return 0;
}
}  // extern "C"
