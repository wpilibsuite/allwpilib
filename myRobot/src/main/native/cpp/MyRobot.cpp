// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/TimedRobot.h>

#include <vector>

#include <networktables/NetworkTableInstance.h>
#include <networktables/StructArrayTopic.h>
#include <networktables/StructTopic.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/glass/Canvas2d.h"
#include "frc/smartdashboard/SmartDashboard.h"
#include "frc/util/Color.h"

class MyRobot : public frc::TimedRobot {
  frc::Canvas2d m_canvas{150, 150};
  frc::Rotation2d m_rotation{units::degree_t(45)};
  nt::StructArrayPublisher<frc::Rotation2d> structTopic =
      nt::NetworkTableInstance::GetDefault()
          .GetStructArrayTopic<frc::Rotation2d>("rot/asdf")
          .Publish();
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {
    frc::SmartDashboard::PutData("Canvas", &m_canvas);

    m_canvas.DrawLine(0, 0, 50, 50, 1, frc::Color::kRed, 255);
    m_canvas.FinishFrame();

    std::vector<frc::Rotation2d> rot{m_rotation};
    structTopic.Set(rot);
  }

  /**
   * This function is run once each time the robot enters autonomous mode
   */
  void AutonomousInit() override {}

  /**
   * This function is called periodically during autonomous
   */
  void AutonomousPeriodic() override {}

  /**
   * This function is called once each time the robot enters tele-operated mode
   */
  void TeleopInit() override {}

  /**
   * This function is called periodically during operator control
   */
  void TeleopPeriodic() override {}

  /**
   * This function is called periodically during test mode
   */
  void TestPeriodic() override {}

  /**
   * This function is called periodically during all modes
   */
  void RobotPeriodic() override {}
};

int main() {
  return frc::StartRobot<MyRobot>();
}
