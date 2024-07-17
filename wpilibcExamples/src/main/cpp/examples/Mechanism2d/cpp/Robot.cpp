// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/AnalogPotentiometer.h>
#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/SimHooks.h>
#include <frc/smartdashboard/Mechanism2d.h>
#include <frc/smartdashboard/MechanismLigament2d.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/util/Color.h>
#include <frc/util/Color8Bit.h>
#include <units/angle.h>

/**
 * This sample program shows how to use Mechanism2d - a visual representation of
 * arms, elevators, and other mechanisms on dashboards; driven by a node-based
 * API.
 *
 * <p>Ligaments are based on other ligaments or roots, and roots are contained
 * in the base Mechanism2d object. Use pointers for nodes, and beware not to let
 * the container out of scope - the appended nodes will be recursively
 * destructed!
 */
class Robot : public frc::TimedRobot {
  static constexpr double kMetersPerPulse = 0.01;
  static constexpr double kElevatorMinimumLength = 0.5;

 public:
  Robot() {
    m_elevatorEncoder.SetDistancePerPulse(kMetersPerPulse);

    // publish to dashboard
    frc::SmartDashboard::PutData("Mech2d", &m_mech);
  }

  void RobotPeriodic() override {
    // update the dashboard mechanism's state
    m_elevator->SetLength(kElevatorMinimumLength +
                          m_elevatorEncoder.GetDistance());
    m_wrist->SetAngle(units::degree_t{m_wristPotentiometer.Get()});
  }

  void TeleopPeriodic() override {
    m_elevatorMotor.Set(m_joystick.GetRawAxis(0));
    m_wristMotor.Set(m_joystick.GetRawAxis(1));
  }

 private:
  frc::PWMSparkMax m_elevatorMotor{0};
  frc::PWMSparkMax m_wristMotor{1};
  frc::Encoder m_elevatorEncoder{0, 1};
  frc::AnalogPotentiometer m_wristPotentiometer{1, 90};
  frc::Joystick m_joystick{0};

  // the main mechanism object
  frc::Mechanism2d m_mech{3, 3};
  // the mechanism root node
  frc::MechanismRoot2d* m_root = m_mech.GetRoot("climber", 2, 0);
  // MechanismLigament2d objects represent each "section"/"stage" of the
  // mechanism, and are based off the root node or another ligament object
  frc::MechanismLigament2d* m_elevator =
      m_root->Append<frc::MechanismLigament2d>("elevator", 1, 90_deg);
  frc::MechanismLigament2d* m_wrist =
      m_elevator->Append<frc::MechanismLigament2d>(
          "wrist", 0.5, 90_deg, 6, frc::Color8Bit{frc::Color::kPurple});
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
