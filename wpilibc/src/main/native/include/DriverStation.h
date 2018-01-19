/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include <HAL/DriverStation.h>
#include <llvm/Twine.h>
#include <support/condition_variable.h>
#include <support/deprecated.h>
#include <support/mutex.h>

#include "ErrorBase.h"
#include "RobotState.h"

namespace frc {

struct MatchInfoData;
class MatchDataSender;

/**
 * Provide access to the network communication data to / from the Driver
 * Station.
 */
class DriverStation : public ErrorBase, public RobotStateInterface {
 public:
  enum Alliance { kRed, kBlue, kInvalid };
  enum MatchType { kNone, kPractice, kQualification, kElimination };

  ~DriverStation() override;
  static DriverStation& GetInstance();
  static void ReportError(const llvm::Twine& error);
  static void ReportWarning(const llvm::Twine& error);
  static void ReportError(bool isError, int code, const llvm::Twine& error,
                          const llvm::Twine& location,
                          const llvm::Twine& stack);

  static constexpr int kJoystickPorts = 6;

  bool GetStickButton(int stick, int button);
  bool GetStickButtonPressed(int stick, int button);
  bool GetStickButtonReleased(int stick, int button);

  double GetStickAxis(int stick, int axis);
  int GetStickPOV(int stick, int pov);
  int GetStickButtons(int stick) const;

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
  WPI_DEPRECATED("Use RobotController static class method")
  bool IsSysActive() const;
  WPI_DEPRECATED("Use RobotController static class method")
  bool IsBrownedOut() const;

  std::string GetGameSpecificMessage() const;
  std::string GetEventName() const;
  MatchType GetMatchType() const;
  int GetMatchNumber() const;
  int GetReplayNumber() const;

  Alliance GetAlliance() const;
  int GetLocation() const;
  void WaitForData();
  bool WaitForData(double timeout);
  double GetMatchTime() const;
  double GetBatteryVoltage() const;

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting disabled code; if false, leaving disabled
   *                 code.
   */
  void InDisabled(bool entering) { m_userInDisabled = entering; }

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting autonomous code; if false, leaving
   *                 autonomous code.
   */
  void InAutonomous(bool entering) { m_userInAutonomous = entering; }

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop
   *                 code.
   */
  void InOperatorControl(bool entering) { m_userInTeleop = entering; }

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting test code; if false, leaving test code.
   */
  void InTest(bool entering) { m_userInTest = entering; }

 protected:
  void GetData();

 private:
  DriverStation();

  void ReportJoystickUnpluggedError(const llvm::Twine& message);
  void ReportJoystickUnpluggedWarning(const llvm::Twine& message);
  void Run();
  void UpdateControlWord(bool force, HAL_ControlWord& controlWord) const;
  void SendMatchData();

  // Joystick User Data
  std::unique_ptr<HAL_JoystickAxes[]> m_joystickAxes;
  std::unique_ptr<HAL_JoystickPOVs[]> m_joystickPOVs;
  std::unique_ptr<HAL_JoystickButtons[]> m_joystickButtons;
  std::unique_ptr<HAL_JoystickDescriptor[]> m_joystickDescriptor;
  std::unique_ptr<MatchInfoData> m_matchInfo;

  // Joystick Cached Data
  std::unique_ptr<HAL_JoystickAxes[]> m_joystickAxesCache;
  std::unique_ptr<HAL_JoystickPOVs[]> m_joystickPOVsCache;
  std::unique_ptr<HAL_JoystickButtons[]> m_joystickButtonsCache;
  std::unique_ptr<HAL_JoystickDescriptor[]> m_joystickDescriptorCache;
  std::unique_ptr<MatchInfoData> m_matchInfoCache;

  std::unique_ptr<MatchDataSender> m_matchDataSender;

  // Joystick button rising/falling edge flags
  std::array<uint32_t, kJoystickPorts> m_joystickButtonsPressed;
  std::array<uint32_t, kJoystickPorts> m_joystickButtonsReleased;

  // Internal Driver Station thread
  std::thread m_dsThread;
  std::atomic<bool> m_isRunning{false};

  wpi::mutex m_waitForDataMutex;
  wpi::condition_variable m_waitForDataCond;
  int m_waitForDataCounter;

  mutable wpi::mutex m_cacheDataMutex;

  // Robot state status variables
  bool m_userInDisabled = false;
  bool m_userInAutonomous = false;
  bool m_userInTeleop = false;
  bool m_userInTest = false;

  // Control word variables
  mutable HAL_ControlWord m_controlWordCache;
  mutable std::chrono::steady_clock::time_point m_lastControlWordUpdate;
  mutable wpi::mutex m_controlWordMutex;

  double m_nextMessageTime = 0;
};

}  // namespace frc
