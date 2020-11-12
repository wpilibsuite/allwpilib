/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/Encoder.h>
#include <frc/GenericHID.h>
#include <frc/Joystick.h>
#include <frc/PWMVictorSPX.h>
#include <frc/RobotController.h>
#include <frc/StateSpaceUtil.h>
#include <frc/TimedRobot.h>
#include <frc/controller/PIDController.h>
#include <frc/simulation/BatterySim.h>
#include <frc/simulation/ElevatorSim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/RoboRioSim.h>
#include <frc/system/plant/LinearSystemId.h>
#include <units/angle.h>
#include <units/moment_of_inertia.h>
#include <wpi/math>

/**
 * This is a sample program to demonstrate how to use a state-space controller
 * to control an arm.
 */
class Robot : public frc::TimedRobot {
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;
  static constexpr int kJoystickPort = 0;

  static constexpr double kElevatorKp = 5.0;
  static constexpr double kElevatorGearing = 10.0;
  static constexpr units::meter_t kElevatorDrumRadius = 2_in;
  static constexpr units::kilogram_t kCarriageMass = 4.0_kg;

  static constexpr units::meter_t kMinElevatorHeight = 0_in;
  static constexpr units::meter_t kMaxElevatorHeight = 50_in;

  // distance per pulse = (distance per revolution) / (pulses per revolution)
  //  = (Pi * D) / ppr
  static constexpr double kArmEncoderDistPerPulse =
      2.0 * wpi::math::pi * kElevatorDrumRadius.to<double>() / 4096.0;

  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  frc::DCMotor m_elevatorGearbox = frc::DCMotor::Vex775Pro(4);

  // Standard classes for controlling our elevator
  frc2::PIDController m_controller{kElevatorKp, 0, 0};
  frc::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};
  frc::PWMVictorSPX m_motor{kMotorPort};
  frc::Joystick m_joystick{kJoystickPort};

  // Simulation classes help us simulate what's going on, including gravity.
  frc::sim::ElevatorSim m_elevatorSim{m_elevatorGearbox,
                                      kElevatorGearing,
                                      kCarriageMass,
                                      kElevatorDrumRadius,
                                      kMinElevatorHeight,
                                      kMaxElevatorHeight,
                                      {0.01}};
  frc::sim::EncoderSim m_encoderSim{m_encoder};

 public:
  void RobotInit() { m_encoder.SetDistancePerPulse(kArmEncoderDistPerPulse); }

  void SimulationPeriodic() {
    // In this method, we update our simulation of what our elevator is doing
    // First, we set our "inputs" (voltages)
    m_elevatorSim.SetInput(frc::MakeMatrix<1, 1>(
        m_motor.Get() * frc::RobotController::GetInputVoltage()));

    // Next, we update it. The standard loop time is 20ms.
    m_elevatorSim.Update(20_ms);

    // Finally, we set our simulated encoder's readings and simulated battery
    // voltage
    m_encoderSim.SetDistance(m_elevatorSim.GetPosition().to<double>());
    // SimBattery estimates loaded battery voltages
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({m_elevatorSim.GetCurrentDraw()}));
  }

  void TeleopPeriodic() {
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

  void DisabledInit() {
    // This just makes sure that our simulation code knows that the motor's off.
    m_motor.Set(0.0);
  }
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
