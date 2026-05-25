// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/AnalogPotentiometer.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
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
    elevatorEncoder.SetDistancePerPulse(kMetersPerPulse);

    // publish to dashboard
    wpi::SmartDashboard::PutData("Mech2d", &mech);
  }

  void RobotPeriodic() override {
    // update the dashboard mechanism's state
    elevator->SetLength(kElevatorMinimumLength + elevatorEncoder.GetDistance());
    wrist->SetAngle(wpi::units::degree_t{wristPotentiometer.Get()});
  }

  void TeleopPeriodic() override {
    elevatorMotor.SetThrottle(joystick.GetRawAxis(0));
    wristMotor.SetThrottle(joystick.GetRawAxis(1));
  }

 private:
  wpi::PWMSparkMax elevatorMotor{0};
  wpi::PWMSparkMax wristMotor{1};
  wpi::Encoder elevatorEncoder{0, 1};
  wpi::AnalogPotentiometer wristPotentiometer{1, 90};
  wpi::Joystick joystick{0};

  // the main mechanism object
  wpi::Mechanism2d mech{3, 3};
  // the mechanism root node
  wpi::MechanismRoot2d* root = mech.GetRoot("climber", 2, 0);
  // MechanismLigament2d objects represent each "section"/"stage" of the
  // mechanism, and are based off the root node or another ligament object
  wpi::MechanismLigament2d* elevator =
      root->Append<wpi::MechanismLigament2d>("elevator", 1, 90_deg);
  wpi::MechanismLigament2d* wrist = elevator->Append<wpi::MechanismLigament2d>(
      "wrist", 0.5, 90_deg, 6, wpi::util::Color8Bit{wpi::util::Color::PURPLE});
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
