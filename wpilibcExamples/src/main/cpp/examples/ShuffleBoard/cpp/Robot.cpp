/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/AnalogPotentiometer.h>
#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/PWMVictorSPX.h>
#include <frc/TimedRobot.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/shuffleboard/ShuffleboardLayout.h>
#include <frc/shuffleboard/ShuffleboardTab.h>
#include <networktables/NetworkTableEntry.h>
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
  void RobotInit() override {
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
        driveBaseTab.GetLayout("List Layout", "Encoders")
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
    m_robotDrive.SetMaxOutput(m_maxSpeed.GetDouble(1.0));
  }

 private:
  frc::PWMVictorSPX m_left{0};
  frc::PWMVictorSPX m_right{1};
  frc::PWMVictorSPX m_elevatorMotor{2};

  frc::DifferentialDrive m_robotDrive{m_left, m_right};

  frc::Joystick m_stick{0};

  frc::Encoder m_leftEncoder{0, 1};
  frc::Encoder m_rightEncoder{2, 3};
  frc::AnalogPotentiometer m_ElevatorPot{0};

  nt::NetworkTableEntry m_maxSpeed;
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
