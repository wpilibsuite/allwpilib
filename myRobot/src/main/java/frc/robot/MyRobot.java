// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StructPublisher;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.glass.Canvas2d;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;

public class MyRobot extends TimedRobot {
  Canvas2d canvas2d = new Canvas2d(100, 100);
  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  @Override
  public void robotInit() {
    SmartDashboard.putData("Canvas2d", canvas2d);

    canvas2d.drawLine(0, 0, 50, 50, 1, new Color8Bit(Color.kRed));
    canvas2d.drawLine(10, 90, 90, 10, 4, new Color8Bit(Color.kBlue));
    canvas2d.finishFrame();

    Rotation2d rotation2d = new Rotation2d(1, 2);

    StructPublisher<Rotation2d> rotation2dPublisher = NetworkTableInstance.getDefault().getStructTopic("Rotation", Rotation2d.struct).publish();
    rotation2dPublisher.set(rotation2d);

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
  public void teleopPeriodic() {}

  /** This function is called periodically during test mode. */
  @Override
  public void testPeriodic() {}

  /** This function is called periodically during all modes. */
  @Override
  public void robotPeriodic() {
  }
}
