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
#include <frc/system/plant/LinearSystemId.h>
#include <units/moment_of_inertia.h>

/**
 * This is a sample program to demonstrate the use of a BangBangController with
 * a flywheel to control speed.
 */
class Robot : public frc::TimedRobot {
 public:
  /**
   * Controls flywheel to a set speed (RPM) controlled by a joystick.
   */
  void TeleopPeriodic() override {
    // Scale setpoint value between 0 and maxSetpointValue
    units::radians_per_second_t setpoint =
        units::math::max(0_rpm, m_joystick.GetRawAxis(0) * kMaxSetpointValue);

    // Set setpoint and measurement of the bang-bang controller
    units::volt_t bangOutput =
        m_bangBangController.Calculate(m_encoder.GetRate(), setpoint.value()) *
        12_V;

    // Controls a motor with the output of the BangBang controller and a
    // feedforward. The feedforward is reduced slightly to avoid overspeeding
    // the shooter.
    m_flywheelMotor.SetVoltage(bangOutput +
                               0.9 * m_feedforward.Calculate(setpoint));
  }

  Robot() {
    // Add bang-bang controller to SmartDashboard and networktables.
    frc::SmartDashboard::PutData("BangBangController", &m_bangBangController);
  }

  /**
   * Update our simulation. This should be run every robot loop in simulation.
   */
  void SimulationPeriodic() override {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated velocities to our simulated encoder
    m_flywheelSim.SetInputVoltage(
        m_flywheelMotor.Get() *
        units::volt_t{frc::RobotController::GetInputVoltage()});
    m_flywheelSim.Update(20_ms);
    m_encoderSim.SetRate(m_flywheelSim.GetAngularVelocity().value());
  }

 private:
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;

  // Max setpoint for joystick control
  static constexpr units::radians_per_second_t kMaxSetpointValue = 6000_rpm;

  // Joystick to control setpoint
  frc::Joystick m_joystick{0};

  frc::PWMSparkMax m_flywheelMotor{kMotorPort};
  frc::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};

  frc::BangBangController m_bangBangController;

  // Gains are for example purposes only - must be determined for your own
  // robot!
  static constexpr units::volt_t kFlywheelKs = 0.0001_V;
  static constexpr decltype(1_V / 1_rad_per_s) kFlywheelKv = 0.000195_V / 1_rpm;
  static constexpr decltype(1_V / 1_rad_per_s_sq) kFlywheelKa =
      0.0003_V / 1_rev_per_m_per_s;
  frc::SimpleMotorFeedforward<units::radians> m_feedforward{
      kFlywheelKs, kFlywheelKv, kFlywheelKa};

  // Simulation classes help us simulate our robot

  // Reduction between motors and encoder, as output over input. If the flywheel
  // spins slower than the motors, this number should be greater than one.
  static constexpr double kFlywheelGearing = 1.0;

  // 1/2 MR²
  static constexpr units::kilogram_square_meter_t kFlywheelMomentOfInertia =
      0.5 * 1.5_lb * 4_in * 4_in;

  frc::DCMotor m_gearbox = frc::DCMotor::NEO(1);
  frc::LinearSystem<1, 1, 1> m_plant{frc::LinearSystemId::FlywheelSystem(
      m_gearbox, kFlywheelMomentOfInertia, kFlywheelGearing)};

  frc::sim::FlywheelSim m_flywheelSim{m_plant, m_gearbox};
  frc::sim::EncoderSim m_encoderSim{m_encoder};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
