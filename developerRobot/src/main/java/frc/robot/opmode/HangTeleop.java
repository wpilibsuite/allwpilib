// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.opmode;

import edu.wpi.first.wpilibj.opmode.PeriodicOpMode;
import edu.wpi.first.wpilibj.opmode.Teleop;

@Teleop
public class HangTeleop extends PeriodicOpMode {
  @Override
  public void periodic() {
    try {
      System.out.println("sleeping");
      Thread.sleep(10000);
      System.out.println("woke up");
    } catch (InterruptedException e) {
      System.out.println("interrupted");
      Thread.currentThread().interrupt();
    }
  }
}
