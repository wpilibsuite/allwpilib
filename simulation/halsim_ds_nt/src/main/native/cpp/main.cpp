/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <HALSimDsNt.h>

static HALSimDSNT dsnt;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  std::cout << "DriverStationNT Initializing." << std::endl;

  dsnt.Initialize();

  std::cout << "DriverStationNT Initialized!" << std::endl;
  return 0;
}
}  // extern "C"
