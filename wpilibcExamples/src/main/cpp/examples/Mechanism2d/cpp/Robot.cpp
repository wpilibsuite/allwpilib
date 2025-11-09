// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/AnalogPotentiometer.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/smartdashboard/Mechanism2d.hpp"
#include "wpi/smartdashboard/MechanismLigament2d.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/util/Color.hpp"
#include "wpi/util/Color8Bit.hpp"

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
class Robot : public wpi::TimedRobot {
  static constexpr double kMetersPerPulse = 0.01;
  static constexpr double kElevatorMinimumLength = 0.5;

 public:
  Robot() {
    m_elevatorEncoder.SetDistancePerPulse(kMetersPerPulse);

    // publish to dashboard
    wpi::SmartDashboard::PutData("Mech2d", &m_mech);
  }

  void RobotPeriodic() override {
    // update the dashboard mechanism's state
    m_elevator->SetLength(kElevatorMinimumLength +
                          m_elevatorEncoder.GetDistance());
    m_wrist->SetAngle(wpi::units::degree_t{m_wristPotentiometer.Get()});
  }

  void TeleopPeriodic() override {
    m_elevatorMotor.Set(m_joystick.GetRawAxis(0));
    m_wristMotor.Set(m_joystick.GetRawAxis(1));
  }

 private:
  wpi::PWMSparkMax m_elevatorMotor{0};
  wpi::PWMSparkMax m_wristMotor{1};
  wpi::Encoder m_elevatorEncoder{0, 1};
  wpi::AnalogPotentiometer m_wristPotentiometer{1, 90};
  wpi::Joystick m_joystick{0};

  // the main mechanism object
  wpi::Mechanism2d m_mech{3, 3};
  // the mechanism root node
  wpi::MechanismRoot2d* m_root = m_mech.GetRoot("climber", 2, 0);
  // MechanismLigament2d objects represent each "section"/"stage" of the
  // mechanism, and are based off the root node or another ligament object
  wpi::MechanismLigament2d* m_elevator =
      m_root->Append<wpi::MechanismLigament2d>("elevator", 1, 90_deg);
  wpi::MechanismLigament2d* m_wrist =
      m_elevator->Append<wpi::MechanismLigament2d>(
          "wrist", 0.5, 90_deg, 6, wpi::Color8Bit{wpi::Color::kPurple});
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
