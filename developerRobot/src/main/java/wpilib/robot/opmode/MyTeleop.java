// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot.opmode;

import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.opmode.Teleop;

@Teleop
public class MyTeleop extends PeriodicOpMode {
  int m_disabledCount;
  int m_periodicCount;

  /** Constructor. */
  public MyTeleop() {
    /*
     * The constructor is called when the opmode is selected on the DS.
     * The robot is typically disabled when this is called (but in a
     * competition match might be enabled).
     *
     * Note: No-arg constructors are okay too.
     *
     * Can call super(period) to set a different periodic time interval.
     *
     * Additional periodic methods may be configured with addPeriodic().
     */
    System.out.println("MyTeleop constructed");
  }

  @Override
  public void disabledPeriodic() {
    /* Called periodically (on every DS packet) while the robot is disabled. */
    m_disabledCount++;
    if (m_disabledCount % 20 == 0) {
      System.out.println("MyTeleop disabled periodic " + m_disabledCount);
    }
  }

  @Override
  public void start() {
    /* Called once when the robot is first enabled. */
    System.out.println("MyTeleop start");
  }

  @Override
  public void periodic() {
    /* Called periodically (set time interval) while the robot is enabled. */
    m_periodicCount++;
    if (m_periodicCount % 20 == 0) {
      System.out.println("MyTeleop periodic " + m_periodicCount);
    }
  }

  @Override
  public void end() {
    /* Called when the robot is disabled (after previously being enabled). */
    System.out.println("MyTeleop end");
  }

  @Override
  public void close() {
    /* Called when the opmode is de-selected / no additional methods will be called. */
    System.out.println("MyTeleop closed");
  }
}
