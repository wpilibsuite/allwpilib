// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ExampleSmartMotorController.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
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
    // Note: These gains are fake, and will have to be tuned for your robot.
    motor.SetPID(1.3, 0.0, 0.7);
  }

  void TeleopPeriodic() override {
    if (joystick.GetRawButtonPressed(2)) {
      goal = {5_m, 0_mps};
    } else if (joystick.GetRawButtonPressed(3)) {
      goal = {0_m, 0_mps};
    }

    // Retrieve the profiled setpoint for the next timestep. This setpoint moves
    // toward the goal while obeying the constraints.
    setpoint = profile.Calculate(kDt, setpoint, goal);

    // Send setpoint to offboard controller PID
    motor.SetSetpoint(ExampleSmartMotorController::PIDMode::kPosition,
                      setpoint.position.value(),
                      feedforward.Calculate(setpoint.velocity) / 12_V);
  }

 private:
  wpi::Joystick joystick{1};
  ExampleSmartMotorController motor{1};
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> feedforward{
      // Note: These gains are fake, and will have to be tuned for your robot.
      1_V, 1.5_V * 1_s / 1_m};

  // Create a motion profile with the given maximum velocity and maximum
  // acceleration constraints for the next setpoint.
  wpi::math::TrapezoidProfile<wpi::units::meters> profile{
      {1.75_mps, 0.75_mps_sq}};
  wpi::math::TrapezoidProfile<wpi::units::meters>::State goal;
  wpi::math::TrapezoidProfile<wpi::units::meters>::State setpoint;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
