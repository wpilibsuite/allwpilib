// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Twist3d;
import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.datalog.StringLogEntry;
import edu.wpi.first.wpilibj.TimedRobot;

public class MyRobot extends TimedRobot {
  private static double[] timePose3dExpMicros(
      StringLogEntry discardEntry, Pose3d start, Twist3d twist, int count) {
    double[] times = new double[count];
    for (int i = 0; i < count; i++) {
      final long startTime = System.nanoTime();
      final var result = start.exp(twist);
      final long endTime = System.nanoTime();
      times[i] = (endTime - startTime) / 1000.0;
      discardEntry.append(result.toString());
    }
    return times;
  }

  private static double[] timePose3dLogMicros(
      StringLogEntry discardEntry, Pose3d start, Pose3d end, int count) {
    double[] times = new double[count];
    for (int i = 0; i < count; i++) {
      final long startTime = System.nanoTime();
      final var result = start.log(end);
      final long endTime = System.nanoTime();
      times[i] = (endTime - startTime) / 1000.0;
      discardEntry.append(result.toString());
    }
    return times;
  }

  private static String summarizeMicros(double[] times) {
    double total = 0;
    for (double time : times) {
      total += time;
    }
    double mean = total / times.length;
    double sumsOfSquares = 0;
    for (double time : times) {
      sumsOfSquares += (time - mean) * (time - mean);
    }
    double stdDev = Math.sqrt(sumsOfSquares / times.length);
    return "mean " + mean + "µs, std dev " + stdDev + "µs";
  }

  private static void timeSuitePose3dExp(StringLogEntry discardEntry) {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var twist = new Twist3d(0, 0, 0, Math.PI, 0, 0);
    final int warmupCount = 100_000;
    final int normalCount = 1_000;
    System.out.println("Pose3d.exp():");
    System.out.println("  start = " + start);
    System.out.println("  twist = " + twist);
    System.out.println(
        "  warm up ("
            + warmupCount
            + " iterations): "
            + summarizeMicros(timePose3dExpMicros(discardEntry, start, twist, warmupCount)));
    System.out.println("  iterations per run: " + normalCount);
    for (int i = 1; i <= 5; i++) {
      System.out.println(
          "  run "
              + i
              + ": "
              + summarizeMicros(timePose3dExpMicros(discardEntry, start, twist, normalCount)));
    }
  }

  private static void timeSuitePose3dLog(StringLogEntry discardEntry) {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var end = new Pose3d(0, 1, 0, new Rotation3d(Math.PI / 2, 0, 0));
    final int warmupCount = 100_000;
    final int normalCount = 1_000;
    System.out.println("Pose3d.log():");
    System.out.println("  start = " + start);
    System.out.println("  end = " + end);
    System.out.println(
        "  warm up ("
            + warmupCount
            + " iterations): "
            + summarizeMicros(timePose3dLogMicros(discardEntry, start, end, warmupCount)));
    System.out.println("  iterations per run: " + normalCount);
    for (int i = 1; i <= 5; i++) {
      System.out.println(
          "  run "
              + i
              + ": "
              + summarizeMicros(timePose3dLogMicros(discardEntry, start, end, normalCount)));
    }
  }

  /** Constructs a MyRobot instance. Also times Pose3d.exp() and Pose3d.log(). */
  public MyRobot() {
    final var datalog = new DataLog();
    final var discardEntry = new StringLogEntry(datalog, "discard");
    timeSuitePose3dExp(discardEntry);
    timeSuitePose3dLog(discardEntry);
  }

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  @Override
  public void robotInit() {}

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
