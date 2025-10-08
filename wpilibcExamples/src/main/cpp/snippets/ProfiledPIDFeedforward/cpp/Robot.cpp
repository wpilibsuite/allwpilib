// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Encoder.h>
#include <frc/TimedRobot.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <units/acceleration.h>
#include <units/length.h>
#include <units/velocity.h>
#include <units/voltage.h>

/**
 * ProfiledPIDController with feedforward snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/controllers/profiled-pidcontroller.html
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() { m_encoder.SetDistancePerPulse(1.0 / 256.0); }

  // Controls a simple motor's position using a SimpleMotorFeedforward
  // and a ProfiledPIDController
  void GoToPosition(units::meter_t goalPosition) {
    auto pidVal = m_controller.Calculate(
        units::meter_t{m_encoder.GetDistance()}, goalPosition);
    m_motor.SetVoltage(pidVal +
                       m_feedforward.Calculate(
                           m_lastSpeed, m_controller.GetSetpoint().velocity));
    m_lastSpeed = m_controller.GetSetpoint().velocity;
  }

  void TeleopPeriodic() override {
    // Example usage: move to position 10.0 meters
    GoToPosition(10.0_m);
  }

 private:
  frc::ProfiledPIDController<units::meters> m_controller{
      1.0, 0.0, 0.0, {5_mps, 10_mps_sq}};
  frc::SimpleMotorFeedforward<units::meters> m_feedforward{0.5_V, 1.5_V / 1_mps,
                                                           0.3_V / 1_mps_sq};
  frc::Encoder m_encoder{0, 1};
  frc::PWMSparkMax m_motor{0};

  units::meters_per_second_t m_lastSpeed = 0_mps;
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
