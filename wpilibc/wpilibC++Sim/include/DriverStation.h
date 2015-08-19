/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SensorBase.h"
#include "RobotState.h"
#include "HAL/HAL.hpp"
#include "HAL/cpp/Semaphore.hpp"
#include "HAL/cpp/priority_mutex.h"
#include "HAL/cpp/priority_condition_variable.h"
#include <condition_variable>

struct HALControlWord;
class AnalogInput;

/**
 * Provide access to the network communication data to / from the Driver
 * Station.
 */
class DriverStation : public SensorBase, public RobotStateInterface {
 public:
  enum Alliance { kRed, kBlue, kInvalid };

  virtual ~DriverStation();
  static DriverStation &GetInstance();
  static void ReportError(std::string error);

  static const uint32_t kJoystickPorts = 6;

  float GetStickAxis(uint32_t stick, uint32_t axis);
  int GetStickPOV(uint32_t stick, uint32_t pov);
  uint32_t GetStickButtons(uint32_t stick) const;
  bool GetStickButton(uint32_t stick, uint8_t button);

  int GetStickAxisCount(uint32_t stick) const;
  int GetStickPOVCount(uint32_t stick) const;
  int GetStickButtonCount(uint32_t stick) const;

  bool GetJoystickIsXbox(uint32_t stick) const;
  int GetJoystickType(uint32_t stick) const;
  std::string GetJoystickName(uint32_t stick) const;
  int GetJoystickAxisType(uint32_t stick, uint8_t axis) const;

  bool IsEnabled() const override;
  bool IsDisabled() const override;
  bool IsAutonomous() const override;
  bool IsOperatorControl() const override;
  bool IsTest() const override;
  bool IsDSAttached() const;
  bool IsNewControlData() const;
  bool IsFMSAttached() const;
  bool IsSysActive() const;
  bool IsSysBrownedOut() const;

  Alliance GetAlliance() const;
  uint32_t GetLocation() const;
  void WaitForData();
  double GetMatchTime() const;
  float GetBatteryVoltage() const;

  /** Only to be used to tell the Driver Station what code you claim to be
   * executing
   *   for diagnostic purposes only
   * @param entering If true, starting disabled code; if false, leaving disabled
   * code */
  void InDisabled(bool entering) { m_userInDisabled = entering; }
  /** Only to be used to tell the Driver Station what code you claim to be
   * executing
   *   for diagnostic purposes only
   * @param entering If true, starting autonomous code; if false, leaving
   * autonomous code */
  void InAutonomous(bool entering) { m_userInAutonomous = entering; }
  /** Only to be used to tell the Driver Station what code you claim to be
   * executing
   *   for diagnostic purposes only
   * @param entering If true, starting teleop code; if false, leaving teleop
   * code */
  void InOperatorControl(bool entering) { m_userInTeleop = entering; }
  /** Only to be used to tell the Driver Station what code you claim to be
   * executing
   *   for diagnostic purposes only
   * @param entering If true, starting test code; if false, leaving test code */
  void InTest(bool entering) { m_userInTest = entering; }

 protected:
  DriverStation();

  void GetData();

 private:
  static DriverStation *m_instance;
  void ReportJoystickUnpluggedError(std::string message);
  void Run();

  HALJoystickAxes m_joystickAxes[kJoystickPorts];
  HALJoystickPOVs m_joystickPOVs[kJoystickPorts];
  HALJoystickButtons m_joystickButtons[kJoystickPorts];
  HALJoystickDescriptor m_joystickDescriptor[kJoystickPorts];
  mutable Semaphore m_newControlData{Semaphore::kEmpty};
  mutable priority_condition_variable m_packetDataAvailableCond;
  priority_mutex m_packetDataAvailableMutex;
  std::condition_variable_any m_waitForDataCond;
  priority_mutex m_waitForDataMutex;
  bool m_userInDisabled = false;
  bool m_userInAutonomous = false;
  bool m_userInTeleop = false;
  bool m_userInTest = false;
  double m_nextMessageTime = 0;
};
