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
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {
    frc::SmartDashboard::PutData("Canvas", &m_canvas);
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
  void RobotPeriodic() override {
    m_canvas.DrawLine(0, 0, 50, 50, 1, frc::Color::kRed, 255);
    m_canvas.DrawCircle(1, 2, 4, 5, false, {255, 255, 255});
    m_canvas.DrawNgon(25, 25, 9, 5, 6, false, {255, 255, 255});
    m_canvas.DrawQuad(25, 25, 25, 50, 50, 50, 50, 25, 3, true, {100, 0, 200});
    m_canvas.DrawText(20, 20, 10, "Hello world", {100, 200, 10}, 100);
    m_canvas.FinishFrame();
  }
};

int main() {
  return frc::StartRobot<MyRobot>();
}
