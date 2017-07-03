/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.Talon;

@SuppressWarnings("all")
public class MyRobot extends IterativeRobot {
  private Talon m_motor1 = new Talon(1);
  private Talon m_motor2 = new Talon(2);
  private SpeedControllerGroup m_group1 = new SpeedControllerGroup(m_motor1, m_motor2);
  private SpeedControllerGroup m_group2 = new SpeedControllerGroup(m_motor1);

  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  @Override
  public void robotInit() {}

  /**
   * This function is run once each time the robot enters autonomous mode
   */
  @Override
  public void autonomousInit() {}

  /**
   * This function is called periodically during autonomous
   */
  @Override
  public void autonomousPeriodic() {}

  /**
   * This function is called once each time the robot enters tele-operated mode
   */
  @Override
  public void teleopInit() {}

  /**
   * This function is called periodically during operator control
   */
  @Override
  public void teleopPeriodic() {}

  /**
   * This function is called periodically during test mode
   */
  @Override
  public void testPeriodic() {}

  /**
   * This function is called periodically during all modes
   */
  @Override
  public void robotPeriodic() {}
}
