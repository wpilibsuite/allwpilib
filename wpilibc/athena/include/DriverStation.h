/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <condition_variable>

#include "HAL/cpp/Semaphore.h"
#include "HAL/cpp/priority_condition_variable.h"
#include "HAL/cpp/priority_mutex.h"
#include "RobotState.h"
#include "SensorBase.h"
#include "Task.h"

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
  static DriverStation& GetInstance();
  static void ReportError(std::string error);
  static void ReportWarning(std::string error);
  static void ReportError(bool is_error, int32_t code, const std::string& error,
                          const std::string& location,
                          const std::string& stack);

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
  bool IsBrownedOut() const;

  Alliance GetAlliance() const;
  uint32_t GetLocation() const;
  void WaitForData();
  double GetMatchTime() const;
  float GetBatteryVoltage() const;

  /** Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only
   * @param entering If true, starting disabled code; if false, leaving disabled
   * code */
  void InDisabled(bool entering) { m_userInDisabled = entering; }
  /** Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only
   * @param entering If true, starting autonomous code; if false, leaving
   * autonomous code */
  void InAutonomous(bool entering) { m_userInAutonomous = entering; }
  /** Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only
   * @param entering If true, starting teleop code; if false, leaving teleop
   * code */
  void InOperatorControl(bool entering) { m_userInTeleop = entering; }
  /** Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only
   * @param entering If true, starting test code; if false, leaving test code */
  void InTest(bool entering) { m_userInTest = entering; }

 protected:
  DriverStation();

  void GetData();

 private:
  static DriverStation* m_instance;
  void ReportJoystickUnpluggedError(std::string message);
  void ReportJoystickUnpluggedWarning(std::string message);
  void Run();

  std::unique_ptr<HALJoystickAxes[]> m_joystickAxes;
  std::unique_ptr<HALJoystickPOVs[]> m_joystickPOVs;
  std::unique_ptr<HALJoystickButtons[]> m_joystickButtons;
  std::unique_ptr<HALJoystickDescriptor[]> m_joystickDescriptor;

  // Cached Data
  std::unique_ptr<HALJoystickAxes[]> m_joystickAxesCache;
  std::unique_ptr<HALJoystickPOVs[]> m_joystickPOVsCache;
  std::unique_ptr<HALJoystickButtons[]> m_joystickButtonsCache;
  std::unique_ptr<HALJoystickDescriptor[]> m_joystickDescriptorCache;

  Task m_task;
  std::atomic<bool> m_isRunning{false};
  mutable Semaphore m_newControlData{Semaphore::kEmpty};
  mutable priority_condition_variable m_packetDataAvailableCond;
  priority_mutex m_packetDataAvailableMutex;
  bool m_updatedControlLoopData = false;
  std::condition_variable_any m_waitForDataCond;
  priority_mutex m_waitForDataMutex;
  mutable priority_mutex m_joystickDataMutex;
  bool m_userInDisabled = false;
  bool m_userInAutonomous = false;
  bool m_userInTeleop = false;
  bool m_userInTest = false;
  double m_nextMessageTime = 0;
};
