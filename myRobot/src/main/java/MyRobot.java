/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

import edu.wpi.first.wpilibj.DistanceEncoder;
import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.SpeedController;
import si.uom.SI;
import systems.uom.common.USCustomary;

@SuppressWarnings("all")
public class MyRobot extends IterativeRobot {

  private DistanceEncoder encoder;
  private SpeedController motor;

  private PIDController autoController;

  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  @Override
  public void robotInit() {
    encoder = new DistanceEncoder(0, 1, 2 * Math.PI, SI.METRE);
    motor = new Spark(0);

    autoController = new PIDController(0.5, 0, 0, encoder.value(), motor, SI.METRE);
  }

  /**
   * This function is run once each time the robot enters autonomous mode
   */
  @Override
  public void autonomousInit() {
    pidAuto();
  }

  /**
   * This function is called periodically during autonomous
   */
  @Override
  public void autonomousPeriodic() {
    //basicAuto();
  }

  private void basicAuto() {
    if (encoder.get(USCustomary.FOOT) < 12) {
      motor.set(1.0);
    } else {
      motor.set(0.0);
    }
  }

  private void pidAuto() {
    autoController.setSetpoint(12, SI.METRE);
    autoController.enable();
  }

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
