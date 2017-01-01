/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <condition_variable>
#include <mutex>
#include <string>

#include <gazebo/transport/transport.hh>

#include "RobotState.h"
#include "SensorBase.h"
#include "llvm/StringRef.h"
#include "simulation/gz_msgs/msgs.h"

#ifdef _WIN32
// Ensure that Winsock2.h is included before Windows.h, which can get
// pulled in by anybody (e.g., Boost).
#include <Winsock2.h>
#endif

namespace frc {

struct HALCommonControlData;
class AnalogInput;

/**
 * Provide access to the network communication data to / from the Driver
 * Station.
 */
class DriverStation : public SensorBase, public RobotStateInterface {
 public:
  enum Alliance { kRed, kBlue, kInvalid };

  virtual ~DriverStation() = default;
  static DriverStation& GetInstance();
  static void ReportError(llvm::StringRef error);
  static void ReportWarning(llvm::StringRef error);
  static void ReportError(bool is_error, int code, llvm::StringRef error,
                          llvm::StringRef location, llvm::StringRef stack);

  static const int kBatteryChannel = 7;
  static const int kJoystickPorts = 4;
  static const int kJoystickAxes = 6;

  double GetStickAxis(int stick, int axis);
  bool GetStickButton(int stick, int button);
  int16_t GetStickButtons(int stick);

  double GetAnalogIn(int channel);
  bool GetDigitalIn(int channel);
  void SetDigitalOut(int channel, bool value);
  bool GetDigitalOut(int channel);

  bool IsEnabled() const;
  bool IsDisabled() const;
  bool IsAutonomous() const;
  bool IsOperatorControl() const;
  bool IsTest() const;
  bool IsFMSAttached() const;

  int GetPacketNumber() const;
  Alliance GetAlliance() const;
  int GetLocation() const;
  void WaitForData();
  bool WaitForData(double timeout);
  double GetMatchTime() const;
  double GetBatteryVoltage() const;
  uint16_t GetTeamNumber() const;

  void IncrementUpdateNumber() { m_updateNumber++; }

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting disabled code; if false, leaving
   *                 disabled code
   */
  void InDisabled(bool entering) { m_userInDisabled = entering; }
  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting autonomous code; if false, leaving
   *                 autonomous code
   */
  void InAutonomous(bool entering) { m_userInAutonomous = entering; }
  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop
   *                 code
   */
  void InOperatorControl(bool entering) { m_userInTeleop = entering; }
  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting test code; if false, leaving test code
   */
  void InTest(bool entering) { m_userInTest = entering; }

 protected:
  DriverStation();

 private:
  static void InitTask(DriverStation* ds);
  static DriverStation* m_instance;
  static int m_updateNumber;
  ///< TODO: Get rid of this and use the semaphore signaling
  static const double kUpdatePeriod;

  void stateCallback(const gazebo::msgs::ConstDriverStationPtr& msg);
  void joystickCallback(const gazebo::msgs::ConstFRCJoystickPtr& msg, int i);
  void joystickCallback0(const gazebo::msgs::ConstFRCJoystickPtr& msg);
  void joystickCallback1(const gazebo::msgs::ConstFRCJoystickPtr& msg);
  void joystickCallback2(const gazebo::msgs::ConstFRCJoystickPtr& msg);
  void joystickCallback3(const gazebo::msgs::ConstFRCJoystickPtr& msg);
  void joystickCallback4(const gazebo::msgs::ConstFRCJoystickPtr& msg);
  void joystickCallback5(const gazebo::msgs::ConstFRCJoystickPtr& msg);

  int m_digitalOut = 0;
  std::condition_variable m_waitForDataCond;
  std::mutex m_waitForDataMutex;
  bool m_updatedControlLoopData = false;
  mutable std::recursive_mutex m_stateMutex;
  std::recursive_mutex m_joystickMutex;
  double m_approxMatchTimeOffset = 0;
  bool m_userInDisabled = false;
  bool m_userInAutonomous = false;
  bool m_userInTeleop = false;
  bool m_userInTest = false;

  gazebo::transport::SubscriberPtr stateSub;
  gazebo::transport::SubscriberPtr joysticksSub[6];
  gazebo::msgs::DriverStationPtr state;
  gazebo::msgs::FRCJoystickPtr joysticks[6];
};

}  // namespace frc
