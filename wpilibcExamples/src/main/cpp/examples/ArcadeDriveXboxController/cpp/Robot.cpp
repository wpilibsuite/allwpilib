/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/GenericHID.h>
#include <frc/PWMVictorSPX.h>
#include <frc/TimedRobot.h>
#include <frc/XboxController.h>
#include <frc/drive/DifferentialDrive.h>

/**
 * This is a demo program showing the use of the DifferentialDrive class.
 * Runs the motors with arcade steering and an Xbox controller.
 */
class Robot : public frc::TimedRobot {
  frc::PWMVictorSPX m_leftMotor{0};
  frc::PWMVictorSPX m_rightMotor{1};
  frc::DifferentialDrive m_robotDrive{m_leftMotor, m_rightMotor};
  frc::XboxController m_driverController{0};

 public:
  void TeleopPeriodic() {
    // Drive with arcade style
    m_robotDrive.ArcadeDrive(
        m_driverController.GetY(frc::GenericHID::JoystickHand::kLeftHand),
        m_driverController.GetX(frc::GenericHID::JoystickHand::kRightHand));
  }
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
