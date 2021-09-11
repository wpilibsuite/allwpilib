// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/RobotController.h>
#include <frc/StateSpaceUtil.h>
#include <frc/TimedRobot.h>
#include <frc/controller/PIDController.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/BatterySim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/RoboRioSim.h>
#include <frc/simulation/SingleJointedArmSim.h>
#include <frc/system/plant/LinearSystemId.h>
#include <units/angle.h>
#include <units/moment_of_inertia.h>
#include <wpi/numbers>

/**
 * This is a sample program to demonstrate how to use a state-space controller
 * to control an arm.
 */
class Robot : public frc::TimedRobot {
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;
  static constexpr int kJoystickPort = 0;

  // The P gain for the PID controller that drives this arm.
  static constexpr double kArmKp = 5.0;

  // distance per pulse = (angle per revolution) / (pulses per revolution)
  //  = (2 * PI rads) / (4096 pulses)
  static constexpr double kArmEncoderDistPerPulse =
      2.0 * wpi::numbers::pi / 4096.0;

  // The arm gearbox represents a gerbox containing two Vex 775pro motors.
  frc::DCMotor m_armGearbox = frc::DCMotor::Vex775Pro(2);

  // Standard classes for controlling our arm
  frc2::PIDController m_controller{kArmKp, 0, 0};
  frc::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};
  frc::PWMSparkMax m_motor{kMotorPort};
  frc::Joystick m_joystick{kJoystickPort};

  // Simulation classes help us simulate what's going on, including gravity.
  // This sim represents an arm with 2 775s, a 100:1 reduction, a mass of 5kg,
  // 30in overall arm length, range of motion nin [-180, 0] degrees, and noise
  // with a standard deviation of 0.5 degrees.
  frc::sim::SingleJointedArmSim m_armSim{
      m_armGearbox,
      100.0,
      frc::sim::SingleJointedArmSim::EstimateMOI(30_in, 5_kg),
      30_in,
      -180_deg,
      0_deg,
      5_kg,
      true,
      {(0.5_deg).to<double>()}};
  frc::sim::EncoderSim m_encoderSim{m_encoder};

 public:
  void RobotInit() override {
    m_encoder.SetDistancePerPulse(kArmEncoderDistPerPulse);
  }

  void SimulationPeriodic() override {
    // In this method, we update our simulation of what our arm is doing
    // First, we set our "inputs" (voltages)
    m_armSim.SetInput(frc::MakeMatrix<1, 1>(
        m_motor.Get() * frc::RobotController::GetInputVoltage()));

    // Next, we update it. The standard loop time is 20ms.
    m_armSim.Update(20_ms);

    // Finally, we set our simulated encoder's readings and simulated battery
    // voltage
    m_encoderSim.SetDistance(m_armSim.GetAngle().to<double>());
    // SimBattery estimates loaded battery voltages
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({m_armSim.GetCurrentDraw()}));
  }

  void TeleopPeriodic() override {
    if (m_joystick.GetTrigger()) {
      // Here, we run PID control like normal, with a constant setpoint of 30in.
      double pidOutput =
          m_controller.Calculate(m_encoder.GetDistance(), (30_in).to<double>());
      m_motor.SetVoltage(units::volt_t(pidOutput));
    } else {
      // Otherwise, we disable the motor.
      m_motor.Set(0.0);
    }
  }

  void DisabledInit() override {
    // This just makes sure that our simulation code knows that the motor's off.
    m_motor.Set(0.0);
  }
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
