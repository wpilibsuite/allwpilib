// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/RobotController.h>
#include <frc/StateSpaceUtil.h>
#include <frc/TimedRobot.h>
#include <frc/controller/ElevatorFeedforward.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/BatterySim.h>
#include <frc/simulation/ElevatorSim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/RoboRioSim.h>
#include <frc/smartdashboard/Mechanism2d.h>
#include <frc/smartdashboard/MechanismLigament2d.h>
#include <frc/smartdashboard/MechanismRoot2d.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/system/plant/LinearSystemId.h>
#include <frc/util/Color.h>
#include <frc/util/Color8Bit.h>
#include <units/angle.h>
#include <units/length.h>
#include <units/moment_of_inertia.h>

/**
 * This is a sample program to demonstrate the use of elevator simulation.
 */
class Robot : public frc::TimedRobot {
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;
  static constexpr int kJoystickPort = 0;

  static constexpr double kElevatorKp = 5.0;
  static constexpr double kElevatorKi = 0.0;
  static constexpr double kElevatorKd = 0.0;

  static constexpr units::volt_t kElevatorkS = 0.0_V;
  static constexpr units::volt_t kElevatorkG = 0.0_V;
  static constexpr auto kElevatorkV = 0.762_V / 1_mps;
  static constexpr auto kElevatorkA = 0.762_V / 1_mps_sq;

  static constexpr double kElevatorGearing = 10.0;
  static constexpr units::meter_t kElevatorDrumRadius = 2_in;
  static constexpr units::kilogram_t kCarriageMass = 4.0_kg;

  static constexpr units::meter_t kSetpoint = 30_in;
  static constexpr units::meter_t kMinElevatorHeight = 2_in;
  static constexpr units::meter_t kMaxElevatorHeight = 50_in;

  // distance per pulse = (distance per revolution) / (pulses per revolution)
  //  = (Pi * D) / ppr
  static constexpr double kArmEncoderDistPerPulse =
      2.0 * std::numbers::pi * kElevatorDrumRadius.value() / 4096.0;

  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  frc::DCMotor m_elevatorGearbox = frc::DCMotor::Vex775Pro(4);

  // Standard classes for controlling our elevator
  frc::TrapezoidProfile<units::meters>::Constraints m_constraints{2.45_mps,
                                                                  2.45_mps_sq};
  frc::ProfiledPIDController<units::meters> m_controller{
      kElevatorKp, kElevatorKi, kElevatorKd, m_constraints};

  frc::ElevatorFeedforward m_feedforward{kElevatorkS, kElevatorkG, kElevatorkV,
                                         kElevatorkA};
  frc::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};
  frc::PWMSparkMax m_motor{kMotorPort};
  frc::Joystick m_joystick{kJoystickPort};

  // Simulation classes help us simulate what's going on, including gravity.
  frc::sim::ElevatorSim m_elevatorSim{m_elevatorGearbox,
                                      kElevatorGearing,
                                      kCarriageMass,
                                      kElevatorDrumRadius,
                                      kMinElevatorHeight,
                                      kMaxElevatorHeight,
                                      true,
                                      {0.01}};
  frc::sim::EncoderSim m_encoderSim{m_encoder};

  // Create a Mechanism2d display of an elevator
  frc::Mechanism2d m_mech2d{20, 50};
  frc::MechanismRoot2d* m_elevatorRoot =
      m_mech2d.GetRoot("Elevator Root", 10, 0);
  frc::MechanismLigament2d* m_elevatorMech2d =
      m_elevatorRoot->Append<frc::MechanismLigament2d>(
          "Elevator", units::inch_t{m_elevatorSim.GetPosition()}.value(),
          90_deg);

 public:
  void RobotInit() override {
    m_encoder.SetDistancePerPulse(kArmEncoderDistPerPulse);

    // Put Mechanism 2d to SmartDashboard
    // To view the Elevator Sim in the simulator, select Network Tables ->
    // SmartDashboard -> Elevator Sim
    frc::SmartDashboard::PutData("Elevator Sim", &m_mech2d);
  }

  void SimulationPeriodic() override {
    // In this method, we update our simulation of what our elevator is doing
    // First, we set our "inputs" (voltages)
    m_elevatorSim.SetInput(frc::Vectord<1>{
        m_motor.Get() * frc::RobotController::GetInputVoltage()});

    // Next, we update it. The standard loop time is 20ms.
    m_elevatorSim.Update(20_ms);

    // Finally, we set our simulated encoder's readings and simulated battery
    // voltage
    m_encoderSim.SetDistance(m_elevatorSim.GetPosition().value());
    // SimBattery estimates loaded battery voltages
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({m_elevatorSim.GetCurrentDraw()}));

    // Update the Elevator length based on the simulated elevator height
    m_elevatorMech2d->SetLength(
        units::inch_t{m_elevatorSim.GetPosition()}.value());
  }

  void TeleopPeriodic() override {
    if (m_joystick.GetTrigger()) {
      // Here, we set the constant setpoint of 30in.
      m_controller.SetGoal(kSetpoint);
    } else {
      // Otherwise, we update the setpoint to 0.
      m_controller.SetGoal(0.0_m);
    }
    // With the setpoint value we run PID control like normal
    double pidOutput =
        m_controller.Calculate(units::meter_t{m_encoder.GetDistance()});
    units::volt_t feedforwardOutput =
        m_feedforward.Calculate(m_controller.GetSetpoint().velocity);
    m_motor.SetVoltage(units::volt_t{pidOutput} + feedforwardOutput);
  }
  // To view the Elevator in the simulator, select Network Tables ->
  // SmartDashboard -> Elevator Sim

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
