/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

namespace frc {

struct CANStatus {
  float percentBusUtilization;
  int busOffCount;
  int txFullCount;
  int receiveErrorCount;
  int transmitErrorCount;
};

class RobotController {
 public:
  RobotController() = delete;
  static int GetFPGAVersion();
  static int64_t GetFPGARevision();
  static uint64_t GetFPGATime();
  static bool GetUserButton();
  static bool IsSysActive();
  static bool IsBrownedOut();
  static double GetInputVoltage();
  static double GetInputCurrent();
  static double GetVoltage3V3();
  static double GetCurrent3V3();
  static bool GetEnabled3V3();
  static int GetFaultCount3V3();
  static double GetVoltage5V();
  static double GetCurrent5V();
  static bool GetEnabled5V();
  static int GetFaultCount5V();
  static double GetVoltage6V();
  static double GetCurrent6V();
  static bool GetEnabled6V();
  static int GetFaultCount6V();
  static CANStatus GetCANStatus();
};
}  // namespace frc
