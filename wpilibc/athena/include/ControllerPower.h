/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

class ControllerPower {
 public:
  static float GetInputVoltage();
  static float GetInputCurrent();
  static float GetVoltage3V3();
  static float GetCurrent3V3();
  static bool GetEnabled3V3();
  static int GetFaultCount3V3();
  static float GetVoltage5V();
  static float GetCurrent5V();
  static bool GetEnabled5V();
  static int GetFaultCount5V();
  static float GetVoltage6V();
  static float GetCurrent6V();
  static bool GetEnabled6V();
  static int GetFaultCount6V();
};
