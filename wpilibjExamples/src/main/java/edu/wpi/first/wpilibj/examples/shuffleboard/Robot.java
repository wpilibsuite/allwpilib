/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.shuffleboard;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.shuffleboard.ShuffleboardLayout;
import edu.wpi.first.wpilibj.shuffleboard.ShuffleboardTab;

public class Robot extends TimedRobot {
  private final DifferentialDrive m_tankDrive = new DifferentialDrive(new PWMVictorSPX(0),
                                                                      new PWMVictorSPX(1));
  private final Encoder m_leftEncoder = new Encoder(0, 1);
  private final Encoder m_rightEncoder = new Encoder(2, 3);

  private final PWMVictorSPX m_elevatorMotor = new PWMVictorSPX(2);
  private final AnalogPotentiometer m_elevatorPot = new AnalogPotentiometer(0);
  private NetworkTableEntry m_maxSpeed;

  @Override
  public void robotInit() {
    // Add a 'max speed' widget to a tab named 'Configuration', using a number slider
    // The widget will be placed in the second column and row and will be two columns wide
    m_maxSpeed = Shuffleboard.getTab("Configuration")
                           .add("Max Speed", 1)
                           .withWidget("Number Slider")
                           .withPosition(1, 1)
                           .withSize(2, 1)
                           .getEntry();

    // Add the tank drive and encoders to a 'Drivebase' tab
    ShuffleboardTab driveBaseTab = Shuffleboard.getTab("Drivebase");
    driveBaseTab.add("Tank Drive", m_tankDrive);
    // Put both encoders in a list layout
    ShuffleboardLayout encoders = driveBaseTab.getLayout("List Layout", "Encoders")
                                              .withPosition(0, 0)
                                              .withSize(2, 2);
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
