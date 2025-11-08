// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/framework/TimedRobot.hpp>
#include <wpi/hardware/motor/PWMSparkMax.hpp>
#include <wpi/hardware/rotation/Encoder.hpp>
#include <wpi/math/controller/ProfiledPIDController.hpp>
#include <wpi/math/controller/SimpleMotorFeedforward.hpp>
#include <wpi/units/acceleration.hpp>
#include <wpi/units/length.hpp>
#include <wpi/units/velocity.hpp>
#include <wpi/units/voltage.hpp>

/**
 * wpi::math::ProfiledPIDController with feedforward snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/controllers/profiled-pidcontroller.html
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() { m_encoder.SetDistancePerPulse(1.0 / 256.0); }

  // Controls a simple motor's position using a
  // wpi::math::SimpleMotorFeedforward and a wpi::math::ProfiledPIDController
  void GoToPosition(wpi::units::meter_t goalPosition) {
    auto pidVal = m_controller.Calculate(
        wpi::units::meter_t{m_encoder.GetDistance()}, goalPosition);
    m_motor.SetVoltage(wpi::units::volt_t{pidVal} +
                       m_feedforward.Calculate(
                           m_lastSpeed, m_controller.GetSetpoint().velocity));
    m_lastSpeed = m_controller.GetSetpoint().velocity;
  }

  void TeleopPeriodic() override {
    // Example usage: move to position 10.0 meters
    GoToPosition(10.0_m);
  }

 private:
  wpi::math::ProfiledPIDController<wpi::units::meters> m_controller{
      1.0, 0.0, 0.0, {5_mps, 10_mps_sq}};
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> m_feedforward{
      0.5_V, 1.5_V / 1_mps, 0.3_V / 1_mps_sq};
  wpi::Encoder m_encoder{0, 1};
  wpi::PWMSparkMax m_motor{0};

  wpi::units::meters_per_second_t m_lastSpeed = 0_mps;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
