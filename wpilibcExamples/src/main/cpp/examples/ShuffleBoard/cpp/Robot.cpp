// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/AnalogPotentiometer.h>
#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/shuffleboard/ShuffleboardLayout.h>
#include <frc/shuffleboard/ShuffleboardTab.h>
#include <networktables/GenericEntry.h>
#include <networktables/NetworkTableInstance.h>

/**
 * This sample program provides an example for ShuffleBoard, an alternative
 * to SmartDashboard for displaying values and properties of different robot
 * parts.
 *
 * ShuffleBoard can use pre-programmed widgets to display various values, such
 * as Boolean Boxes, Sliders, Graphs, and more. In addition, they can display
 * things in various Tabs.
 *
 * For more information on how to create personal layouts and more in
 * ShuffleBoard, feel free to reference the official FIRST WPILib documentation
 * online.
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    wpi::SendableRegistry::AddChild(&m_robotDrive, &m_left);
    wpi::SendableRegistry::AddChild(&m_robotDrive, &m_right);

    // Add a widget titled 'Max Speed' with a number slider.
    m_maxSpeed = frc::Shuffleboard::GetTab("Configuration")
                     .Add("Max Speed", 1)
                     .WithWidget("Number Slider")
                     .GetEntry();

    // Create a 'DriveBase' tab and add the drivetrain object to it.
    frc::ShuffleboardTab& driveBaseTab = frc::Shuffleboard::GetTab("DriveBase");
    driveBaseTab.Add("TankDrive", m_robotDrive);

    // Put encoders in a list layout.
    frc::ShuffleboardLayout& encoders =
        driveBaseTab.GetLayout("Encoders", frc::BuiltInLayouts::kList)
            .WithPosition(0, 0)
            .WithSize(2, 2);
    encoders.Add("Left Encoder", m_leftEncoder);
    encoders.Add("Right Encoder", m_rightEncoder);

    // Create a 'Elevator' tab and add the potentiometer and elevator motor to
    // it.
    frc::ShuffleboardTab& elevatorTab = frc::Shuffleboard::GetTab("Elevator");
    elevatorTab.Add("Motor", m_elevatorMotor);
    elevatorTab.Add("Potentiometer", m_ElevatorPot);
  }

  void AutonomousInit() override {
    // Update the Max Output for the drivetrain.
    m_robotDrive.SetMaxOutput(m_maxSpeed->GetDouble(1.0));
  }

 private:
  frc::PWMSparkMax m_left{0};
  frc::PWMSparkMax m_right{1};
  frc::PWMSparkMax m_elevatorMotor{2};

  frc::DifferentialDrive m_robotDrive{
      [&](double output) { m_left.Set(output); },
      [&](double output) { m_right.Set(output); }};

  frc::Joystick m_stick{0};

  frc::Encoder m_leftEncoder{0, 1};
  frc::Encoder m_rightEncoder{2, 3};
  frc::AnalogPotentiometer m_ElevatorPot{0};

  nt::GenericEntry* m_maxSpeed;
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
