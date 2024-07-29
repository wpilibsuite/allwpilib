// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.shuffleboard;

import edu.wpi.first.networktables.GenericEntry;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj.shuffleboard.BuiltInLayouts;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.shuffleboard.ShuffleboardLayout;
import edu.wpi.first.wpilibj.shuffleboard.ShuffleboardTab;

public class Robot extends TimedRobot {
  private final PWMSparkMax m_leftDriveMotor = new PWMSparkMax(0);
  private final PWMSparkMax m_rightDriveMotor = new PWMSparkMax(1);
  private final DifferentialDrive m_tankDrive =
      new DifferentialDrive(m_leftDriveMotor::set, m_rightDriveMotor::set);
  private final Encoder m_leftEncoder = new Encoder(0, 1);
  private final Encoder m_rightEncoder = new Encoder(2, 3);

  private final PWMSparkMax m_elevatorMotor = new PWMSparkMax(2);
  private final AnalogPotentiometer m_elevatorPot = new AnalogPotentiometer(0);
  private final GenericEntry m_maxSpeed;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    SendableRegistry.addChild(m_tankDrive, m_leftDriveMotor);
    SendableRegistry.addChild(m_tankDrive, m_rightDriveMotor);

    // Add a 'max speed' widget to a tab named 'Configuration', using a number slider
    // The widget will be placed in the second column and row and will be TWO columns wide
    m_maxSpeed =
        Shuffleboard.getTab("Configuration")
            .add("Max Speed", 1)
            .withWidget("Number Slider")
            .withPosition(1, 1)
            .withSize(2, 1)
            .getEntry();

    // Add the tank drive and encoders to a 'Drivebase' tab
    ShuffleboardTab driveBaseTab = Shuffleboard.getTab("Drivebase");
    driveBaseTab.add("Tank Drive", m_tankDrive);
    // Put both encoders in a list layout
    ShuffleboardLayout encoders =
        driveBaseTab.getLayout("Encoders", BuiltInLayouts.kList).withPosition(0, 0).withSize(2, 2);
    encoders.add("Left Encoder", m_leftEncoder);
    encoders.add("Right Encoder", m_rightEncoder);

    // Add the elevator motor and potentiometer to an 'Elevator' tab
    ShuffleboardTab elevatorTab = Shuffleboard.getTab("Elevator");
    elevatorTab.add("Motor", m_elevatorMotor);
    elevatorTab.add("Potentiometer", m_elevatorPot);
  }

  @Override
  public void autonomousInit() {
    // Read the value of the 'max speed' widget from the dashboard
    m_tankDrive.setMaxOutput(m_maxSpeed.getDouble(1.0));
  }
}
