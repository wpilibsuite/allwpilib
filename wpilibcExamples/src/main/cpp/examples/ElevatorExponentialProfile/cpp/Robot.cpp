// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <wpi/driverstation/Joystick.hpp>
#include <wpi/framework/TimedRobot.hpp>
#include <wpi/math/controller/SimpleMotorFeedforward.hpp>
#include <wpi/math/trajectory/ExponentialProfile.hpp>
#include <wpi/units/acceleration.hpp>
#include <wpi/units/length.hpp>
#include <wpi/units/time.hpp>
#include <wpi/units/velocity.hpp>
#include <wpi/units/voltage.hpp>

#include "ExampleSmartMotorController.hpp"

class Robot : public wpi::TimedRobot {
 public:
  static constexpr wpi::units::second_t kDt = 20_ms;

  Robot() {
    // Note: These gains are fake, and will have to be tuned for your robot.
    m_motor.SetPID(1.3, 0.0, 0.7);
  }

  void TeleopPeriodic() override {
    if (m_joystick.GetRawButtonPressed(2)) {
      m_goal = {5_m, 0_mps};
    } else if (m_joystick.GetRawButtonPressed(3)) {
      m_goal = {0_m, 0_mps};
    }

    // Retrieve the profiled setpoint for the next timestep. This setpoint moves
    // toward the goal while obeying the constraints.
    auto next = m_profile.Calculate(kDt, m_goal, m_setpoint);

    // Send setpoint to offboard controller PID
    m_motor.SetSetpoint(
        ExampleSmartMotorController::PIDMode::kPosition,
        m_setpoint.position.value(),
        m_feedforward.Calculate(m_setpoint.velocity, next.velocity) / 12_V);

    m_setpoint = next;
  }

 private:
  wpi::Joystick m_joystick{1};
  ExampleSmartMotorController m_motor{1};
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> m_feedforward{
      // Note: These gains are fake, and will have to be tuned for your robot.
      1_V, 1_V / 1_mps, 1_V / 1_mps_sq};

  // Create a motion profile with the given maximum velocity and maximum
  // acceleration constraints for the next setpoint.
  wpi::math::ExponentialProfile<wpi::units::meters, wpi::units::volts>
      m_profile{{10_V, 1_V / 1_mps, 1_V / 1_mps_sq}};
  wpi::math::ExponentialProfile<wpi::units::meters, wpi::units::volts>::State
      m_goal;
  wpi::math::ExponentialProfile<wpi::units::meters, wpi::units::volts>::State
      m_setpoint;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
