/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

import edu.wpi.first.wpilibj.CounterBase;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.TimedRobot;

@SuppressWarnings("all")
public class MyRobot extends TimedRobot {
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
    private final Encoder m_encoder = new Encoder(1, 2, false, CounterBase.EncodingType.k4X);
  @Override
  public void robotInit() {
  /*
       * Defines the number of samples to average when determining the rate.
       * On a quadrature encoder, values range from 1-255;
       * larger values result in smoother but potentially
       * less accurate rates than lower values.
       */
      m_encoder.setSamplesToAverage(5);

      /*
       * Defines how far the mechanism attached to the encoder moves per pulse. In
       * this case, we assume that a 360 count encoder is directly
       * attached to a 3 inch diameter (1.5inch radius) wheel,
       * and that we want to measure distance in inches.
       */
      m_encoder.setDistancePerPulse(1.0 / 360.0 * 2.0 * Math.PI * 1.5);

      /*
       * Defines the lowest rate at which the encoder will
       * not be considered stopped, for the purposes of
       * the GetStopped() method. Units are in distance / second,
       * where distance refers to the units of distance
       * that you are using, in this case inches.
       */
      m_encoder.setMinRate(1.0);
  }

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
