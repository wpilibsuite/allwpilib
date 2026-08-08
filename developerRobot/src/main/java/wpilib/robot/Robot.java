// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.framework.TimedRobot;
import org.wpilib.math.controller.PIDController;
import org.wpilib.tunable.Selectable;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.Tunables;
import org.wpilib.units.Units;
import org.wpilib.units.measure.Distance;

public class Robot extends TimedRobot {
  final PIDController m_controller = new PIDController(1.0, 0.0, 0.0);
  final Tunable<Distance> m_distance = Tunable.create(Units.Meter.of(0.0));
  final Selectable<String> m_selector = new Selectable<>();

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    Tunables.publish("PID", m_controller);
    Tunables.publish("Distance", m_distance);
    m_selector.add("Option 1", "1");
    m_selector.add("Option 2", "2");
    m_selector.setDefault("Option 2");
    Tunables.publish("Selector", m_selector);
  }

  /** This function is run once each time the robot enters autonomous mode. */
  @Override
  public void autonomousInit() {}

  /** This function is called periodically during autonomous. */
  @Override
  public void autonomousPeriodic() {}

  /** This function is called once each time the robot enters tele-operated mode. */
  @Override
  public void teleopInit() {}

  /** This function is called periodically during operator control. */
  @Override
  public void teleopPeriodic() {
    System.err.println("Distance: " + m_distance.get());
    System.err.println("Selected Option: " + m_selector.getSelected());
  }

  /** This function is called periodically during utility mode. */
  @Override
  public void utilityPeriodic() {}

  /** This function is called periodically during all modes. */
  @Override
  public void robotPeriodic() {}
}
