/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include <HAL/DriverStation.h>
#include <HAL/cpp/priority_mutex.h>
#include <llvm/StringRef.h>

#include "RobotState.h"
#include "SensorBase.h"

namespace frc {

/**
 * Provide access to the network communication data to / from the Driver
 * Station.
 */
class DriverStation : public SensorBase, public RobotStateInterface {
 public:
  enum Alliance { kRed, kBlue, kInvalid };

  virtual ~DriverStation();
  static DriverStation& GetInstance();
  static void ReportError(llvm::StringRef error);
  static void ReportWarning(llvm::StringRef error);
  static void ReportError(bool is_error, int code, llvm::StringRef error,
                          llvm::StringRef location, llvm::StringRef stack);

  static const int kJoystickPorts = 6;

  double GetStickAxis(int stick, int axis);
  int GetStickPOV(int stick, int pov);
  int GetStickButtons(int stick) const;
  bool GetStickButton(int stick, int button);

  int GetStickAxisCount(int stick) const;
  int GetStickPOVCount(int stick) const;
  int GetStickButtonCount(int stick) const;

  bool GetJoystickIsXbox(int stick) const;
  int GetJoystickType(int stick) const;
  std::string GetJoystickName(int stick) const;
  int GetJoystickAxisType(int stick, int axis) const;

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
  int GetLocation() const;
  void WaitForData();
  bool WaitForData(double timeout);
  double GetMatchTime() const;
  double GetBatteryVoltage() const;

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
  void GetData();

 private:
  DriverStation();
  void ReportJoystickUnpluggedError(llvm::StringRef message);
  void ReportJoystickUnpluggedWarning(llvm::StringRef message);
  void Run();
  void UpdateControlWord(bool force, HAL_ControlWord& controlWord) const;

  // Joystick User Data
  std::unique_ptr<HAL_JoystickAxes[]> m_joystickAxes;
  std::unique_ptr<HAL_JoystickPOVs[]> m_joystickPOVs;
  std::unique_ptr<HAL_JoystickButtons[]> m_joystickButtons;
  std::unique_ptr<HAL_JoystickDescriptor[]> m_joystickDescriptor;

  // Joystick Cached Data
  std::unique_ptr<HAL_JoystickAxes[]> m_joystickAxesCache;
  std::unique_ptr<HAL_JoystickPOVs[]> m_joystickPOVsCache;
  std::unique_ptr<HAL_JoystickButtons[]> m_joystickButtonsCache;
  std::unique_ptr<HAL_JoystickDescriptor[]> m_joystickDescriptorCache;

  // Internal Driver Station thread
  std::thread m_dsThread;
  std::atomic<bool> m_isRunning{false};

  mutable hal::priority_mutex m_joystickDataMutex;

  // Robot state status variables
  bool m_userInDisabled = false;
  bool m_userInAutonomous = false;
  bool m_userInTeleop = false;
  bool m_userInTest = false;

  // Control word variables
  mutable HAL_ControlWord m_controlWordCache;
  mutable std::chrono::steady_clock::time_point m_lastControlWordUpdate;
  mutable hal::priority_mutex m_controlWordMutex;

  double m_nextMessageTime = 0;
};

}  // namespace frc
