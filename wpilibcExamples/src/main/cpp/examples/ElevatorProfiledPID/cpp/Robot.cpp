// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/ElevatorFeedforward.hpp"
#include "wpi/math/controller/ProfiledPIDController.hpp"
#include "wpi/math/trajectory/TrapezoidProfile.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

class Robot : public wpi::TimedRobot {
 public:
  static constexpr wpi::units::second_t kDt = 20_ms;

  Robot() {
    encoder.SetDistancePerPulse(1.0 / 360.0 * 2.0 * std::numbers::pi * 1.5);
  }

  void TeleopPeriodic() override {
    if (joystick.GetRawButtonPressed(2)) {
      controller.SetGoal(5_m);
    } else if (joystick.GetRawButtonPressed(3)) {
      controller.SetGoal(0_m);
    }

    // Run controller and update motor output
    motor.SetVoltage(wpi::units::volt_t{controller.Calculate(
                         wpi::units::meter_t{encoder.GetDistance()})} +
                     feedforward.Calculate(controller.GetSetpoint().velocity));
  }

 private:
  static constexpr wpi::units::meters_per_second_t kMaxVelocity = 1.75_mps;
  static constexpr wpi::units::meters_per_second_squared_t kMaxAcceleration =
      0.75_mps_sq;
  static constexpr double kP = 1.3;
  static constexpr double kI = 0.0;
  static constexpr double kD = 0.7;
  static constexpr wpi::units::volt_t kS = 1.1_V;
  static constexpr wpi::units::volt_t kG = 1.2_V;
  static constexpr auto kV = 1.3_V / 1_mps;

  wpi::Joystick joystick{1};
  wpi::Encoder encoder{1, 2};
  wpi::PWMSparkMax motor{1};

  // Create a PID controller whose setpoint's change is subject to maximum
  // velocity and acceleration constraints.
  wpi::math::TrapezoidProfile<wpi::units::meters>::Constraints constraints{
      kMaxVelocity, kMaxAcceleration};
  wpi::math::ProfiledPIDController<wpi::units::meters> controller{
      kP, kI, kD, constraints, kDt};
  wpi::math::ElevatorFeedforward feedforward{kS, kG, kV};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
