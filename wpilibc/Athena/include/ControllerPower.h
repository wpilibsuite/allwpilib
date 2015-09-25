/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __CONTROLLER_POWER_H__
#define __CONTROLLER_POWER_H__

class ControllerPower {
 public:
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
};
#endif