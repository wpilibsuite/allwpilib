// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StructArrayPublisher;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.glass.Canvas2d;
import edu.wpi.first.wpilibj.glass.CanvasLine2d;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;

public class MyRobot extends TimedRobot {
  Canvas2d m_canvas2D = new Canvas2d(150, 150);

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  @Override
  public void robotInit() {
    SmartDashboard.putData("Canvas2d", m_canvas2D);

    Rotation2d rotation2d = new Rotation2d(1, 2);

    StructArrayPublisher<Rotation2d> rotation2dPublisher =
        NetworkTableInstance.getDefault().getStructArrayTopic("a", Rotation2d.struct).publish();
    rotation2dPublisher.set(new Rotation2d[] {rotation2d});

    StructArrayPublisher<CanvasLine2d> canvasLine2dPublisher =
        NetworkTableInstance.getDefault().getStructArrayTopic("b", CanvasLine2d.struct).publish();
    canvasLine2dPublisher.set(
        new CanvasLine2d[] {new CanvasLine2d(1, 2, 3, 4, 5, new Color8Bit(Color.kRed), 255, 0)});

    m_canvas2D.drawLine(0, 0, 50, 50, 1, new Color8Bit(Color.kRed), 255);
    m_canvas2D.drawRect(50, 50, 100, 100, 0, true, new Color8Bit(Color.kGreen), 255);
    m_canvas2D.drawCircle(105, 105, 10, 3, false, new Color8Bit(Color.kBlue), 100);
    m_canvas2D.drawNgon(120, 120, 10, 5, 1, false, new Color8Bit(Color.kYellow), 255);
    m_canvas2D.drawText(5, 5, 5, "Hello world", new Color8Bit(255, 2, 51), 255, 0);
    m_canvas2D.finishFrame();
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
  public void robotPeriodic() {}
}
