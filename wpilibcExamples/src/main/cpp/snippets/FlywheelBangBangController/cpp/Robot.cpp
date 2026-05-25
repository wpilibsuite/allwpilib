// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/BangBangController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/simulation/EncoderSim.hpp"
#include "wpi/simulation/FlywheelSim.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/units/moment_of_inertia.hpp"

/**
 * This is a sample program to demonstrate the use of a
 * wpi::math::BangBangController with a flywheel to control velocity.
 */
class Robot : public wpi::TimedRobot {
 public:
  /**
   * Controls flywheel to a set velocity (RPM) controlled by a joystick.
   */
  void TeleopPeriodic() override {
    // Scale setpoint value between 0 and maxSetpointValue
    wpi::units::radians_per_second_t setpoint = wpi::units::math::max(
        0_rpm, joystick.GetRawAxis(0) * kMaxSetpointValue);

    // Set setpoint and measurement of the bang-bang controller
    wpi::units::volt_t bangOutput =
        bangBangController.Calculate(encoder.GetRate(), setpoint.value()) *
        12_V;

    // Controls a motor with the output of the BangBang controller and a
    // feedforward. The feedforward is reduced slightly to avoid overspeeding
    // the shooter.
    flywheelMotor.SetVoltage(bangOutput +
                             0.9 * feedforward.Calculate(setpoint));
  }

  Robot() {
    // Add bang-bang controller to SmartDashboard and networktables.
    wpi::SmartDashboard::PutData("BangBangController", &bangBangController);
  }

  /**
   * Update our simulation. This should be run every robot loop in simulation.
   */
  void SimulationPeriodic() override {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated velocities to our simulated encoder
    flywheelSim.SetInputVoltage(
        flywheelMotor.GetThrottle() *
        wpi::units::volt_t{wpi::RobotController::GetInputVoltage()});
    flywheelSim.Update(20_ms);
    encoderSim.SetRate(flywheelSim.GetAngularVelocity().value());
  }

 private:
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;

  // Max setpoint for joystick control
  static constexpr wpi::units::radians_per_second_t kMaxSetpointValue =
      6000_rpm;

  // Joystick to control setpoint
  wpi::Joystick joystick{0};

  wpi::PWMSparkMax flywheelMotor{kMotorPort};
  wpi::Encoder encoder{kEncoderAChannel, kEncoderBChannel};

  wpi::math::BangBangController bangBangController;

  // Gains are for example purposes only - must be determined for your own
  // robot!
  static constexpr wpi::units::volt_t kFlywheelKs = 0.0001_V;
  static constexpr decltype(1_V / 1_rad_per_s) kFlywheelKv = 0.000195_V / 1_rpm;
  static constexpr decltype(1_V / 1_rad_per_s_sq) kFlywheelKa =
      0.0003_V / 1_rev_per_m_per_s;
  wpi::math::SimpleMotorFeedforward<wpi::units::radians> feedforward{
      kFlywheelKs, kFlywheelKv, kFlywheelKa};

  // Simulation classes help us simulate our robot

  // Reduction between motors and encoder, as output over input. If the flywheel
  // spins slower than the motors, this number should be greater than one.
  static constexpr double kFlywheelGearing = 1.0;

  // 1/2 MR²
  static constexpr wpi::units::kilogram_square_meter_t
      kFlywheelMomentOfInertia = 0.5 * 1.5_lb * 4_in * 4_in;

  wpi::math::DCMotor gearbox = wpi::math::DCMotor::NEO(1);
  wpi::math::LinearSystem<1, 1, 1> plant{
      wpi::math::Models::FlywheelFromPhysicalConstants(
          gearbox, kFlywheelMomentOfInertia, kFlywheelGearing)};

  wpi::sim::FlywheelSim flywheelSim{plant, gearbox};
  wpi::sim::EncoderSim encoderSim{encoder};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
