// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/Preferences.h>
#include <frc/RobotController.h>
#include <frc/TimedRobot.h>
#include <frc/controller/PIDController.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/BatterySim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/RoboRioSim.h>
#include <frc/simulation/SingleJointedArmSim.h>
#include <frc/smartdashboard/Mechanism2d.h>
#include <frc/smartdashboard/MechanismLigament2d.h>
#include <frc/smartdashboard/MechanismRoot2d.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/system/plant/LinearSystemId.h>
#include <frc/util/Color.h>
#include <frc/util/Color8Bit.h>
#include <units/angle.h>
#include <units/moment_of_inertia.h>

/**
 * This is a sample program to demonstrate how to use a state-space controller
 * to control an arm.
 */
class Robot : public frc::TimedRobot {
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;
  static constexpr int kJoystickPort = 0;

  static constexpr std::string_view kArmPositionKey = "ArmPosition";
  static constexpr std::string_view kArmPKey = "ArmP";

  // The P gain for the PID controller that drives this arm.
  double kArmKp = 50.0;

  units::degree_t armPosition = 75.0_deg;

  // distance per pulse = (angle per revolution) / (pulses per revolution)
  //  = (2 * PI rads) / (4096 pulses)
  static constexpr double kArmEncoderDistPerPulse =
      2.0 * std::numbers::pi / 4096.0;

  // The arm gearbox represents a gearbox containing two Vex 775pro motors.
  frc::DCMotor m_armGearbox = frc::DCMotor::Vex775Pro(2);

  // Standard classes for controlling our arm
  frc2::PIDController m_controller{kArmKp, 0, 0};
  frc::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};
  frc::PWMSparkMax m_motor{kMotorPort};
  frc::Joystick m_joystick{kJoystickPort};

  // Simulation classes help us simulate what's going on, including gravity.
  // This sim represents an arm with 2 775s, a 600:1 reduction, a mass of 5kg,
  // 30in overall arm length, range of motion in [-75, 255] degrees, and noise
  // with a standard deviation of 1 encoder tick.
  frc::sim::SingleJointedArmSim m_armSim{
      m_armGearbox,
      600.0,
      frc::sim::SingleJointedArmSim::EstimateMOI(30_in, 5_kg),
      30_in,
      -75_deg,
      255_deg,
      5_kg,
      true,
      {kArmEncoderDistPerPulse}};
  frc::sim::EncoderSim m_encoderSim{m_encoder};

  // Create a Mechanism2d display of an Arm
  frc::Mechanism2d m_mech2d{60, 60};
  frc::MechanismRoot2d* m_armBase = m_mech2d.GetRoot("ArmBase", 30, 30);
  frc::MechanismLigament2d* m_armTower =
      m_armBase->Append<frc::MechanismLigament2d>(
          "Arm Tower", 30, -90_deg, 6, frc::Color8Bit{frc::Color::kBlue});
  frc::MechanismLigament2d* m_arm = m_armBase->Append<frc::MechanismLigament2d>(
      "Arm", 30, m_armSim.GetAngle(), 6, frc::Color8Bit{frc::Color::kYellow});

 public:
  void RobotInit() override {
    m_encoder.SetDistancePerPulse(kArmEncoderDistPerPulse);

    // Put Mechanism 2d to SmartDashboard
    frc::SmartDashboard::PutData("Arm Sim", &m_mech2d);

    // Set the Arm position setpoint and P constant to Preferences if the keys
    // don't already exist
    if (!frc::Preferences::ContainsKey(kArmPositionKey)) {
      frc::Preferences::SetDouble(kArmPositionKey, armPosition.value());
    }
    if (!frc::Preferences::ContainsKey(kArmPKey)) {
      frc::Preferences::SetDouble(kArmPKey, kArmKp);
    }
  }

  void SimulationPeriodic() override {
    // In this method, we update our simulation of what our arm is doing
    // First, we set our "inputs" (voltages)
    m_armSim.SetInput(frc::Vectord<1>{m_motor.Get() *
                                      frc::RobotController::GetInputVoltage()});

    // Next, we update it. The standard loop time is 20ms.
    m_armSim.Update(20_ms);

    // Finally, we set our simulated encoder's readings and simulated battery
    // voltage
    m_encoderSim.SetDistance(m_armSim.GetAngle().value());
    // SimBattery estimates loaded battery voltages
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({m_armSim.GetCurrentDraw()}));

    // Update the Mechanism Arm angle based on the simulated arm angle
    m_arm->SetAngle(m_armSim.GetAngle());
  }

  void TeleopInit() override {
    // Read Preferences for Arm setpoint and kP on entering Teleop
    armPosition = units::degree_t{
        frc::Preferences::GetDouble(kArmPositionKey, armPosition.value())};
    if (kArmKp != frc::Preferences::GetDouble(kArmPKey, kArmKp)) {
      kArmKp = frc::Preferences::GetDouble(kArmPKey, kArmKp);
      m_controller.SetP(kArmKp);
    }
  }

  void TeleopPeriodic() override {
    if (m_joystick.GetTrigger()) {
      // Here, we run PID control like normal, with a setpoint read from
      // preferences in degrees.
      double pidOutput = m_controller.Calculate(
          m_encoder.GetDistance(), (units::radian_t{armPosition}.value()));
      m_motor.SetVoltage(units::volt_t{pidOutput});
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
