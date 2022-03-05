// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/controller/BangBangController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/FlywheelSim.h>
#include <frc/smartdashboard/SmartDashboard.h>

/**
 * This is a sample program to demonstrate the use of a BangBangController with
 * a flywheel to control speed.
 */
class Robot : public frc::TimedRobot {
 public:
  /**
   * Controls flywheel to a set speed (rad/s) controlled by a joystick
   */
  void TeleopPeriodic() override {
    // Scale setpoint value between 0 and maxSetpointValue
    units::radians_per_second_t setpoint{
        std::max(0.0, m_joystick.GetRawAxis(0) * kMaxSetpointValue)};
    // Set setpoint and measurement of the bang bang controller
    double bangOutput =
        m_bangBangControler.Calculate(m_encoder.GetRate(), setpoint.value());

    // Controls a motor with the output of the BangBang controller and a
    // feedforward Shrinks the feedforward slightly to avoid overspeeding the
    // shooter
    double feedf = m_feedforward.Calculate(setpoint).value();
    m_flywheelMotor.Set(bangOutput + 0.9 * feedf);
  }

  void RobotInit() override {
    // Add bang bang controler to SmartDashboard and networktables.
    frc::SmartDashboard::PutData("BangBangControler", &m_bangBangControler);
  }
  /** Update our simulation. This should be run every robot loop in simulation.
   */
  void SimulationPeriodic() override {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated velocities to our simulated encoder
    m_flywheelSim.SetInputVoltage(units::volt_t{m_flywheelMotor.Get()} *
                                  frc::RobotController::GetInputVoltage());
    m_flywheelSim.Update(0.02_s);
    m_encoderSim.SetRate(m_flywheelSim.GetAngularVelocity().value());
  }

 private:
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;

  static constexpr double kMaxSetpointValue =
      630;  // Max value for joystick control (rad/s)

  frc::PWMSparkMax m_flywheelMotor{kMotorPort};
  frc::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};
  frc::BangBangController m_bangBangControler{};
  // Gains are for example purposes only - must be determined for your own
  // robot!
  frc::SimpleMotorFeedforward<units::radians> m_feedforward{
      0.000954_V, 0.00186_V / 1_rad_per_s, 0.00286_V / 1_rad_per_s_sq};
  frc::Joystick m_joystick{0};  // Joystick to control setpoint

  // Simulation classes help us simulate our robot

  // Reduction between motors and encoder, as output over input. If the flywheel
  // spins slower than the motors, this number should be greater than one.
  static constexpr double kFlywheelGearing = 1.0;
  frc::sim::FlywheelSim m_flywheelSim{
      frc::DCMotor::NEO(1), kFlywheelGearing,
      0.5 * 1.5_lb * units::math::pow<2>(4_in)};  // 1/2*M*R^2
  frc::sim::EncoderSim m_encoderSim{m_encoder};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
