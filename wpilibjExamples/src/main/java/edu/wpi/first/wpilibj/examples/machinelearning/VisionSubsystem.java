/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.machinelearning;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class VisionSubsystem extends SubsystemBase {
  private NetworkTable table;
  private int totalObjects = 0, totalBalls = 0;
  private Ball[] balls = new Ball[0];
  private String[] classes;
  private double[] boxes, box;
  private NetworkTableEntry totalObjectsEntry, classesEntry, boxesEntry;

  private static abstract class Gamepiece {
    public double distance, xOffset;

    /**
     * Gets the relative angle of the game piece in radians
     * 
     * @return the angle
     */
    public double getAngle() {
      return Math.atan(xOffset / distance);
    }
  }

  /**
   * Represents a detected cargo from the Coral
   */
  public static class Ball extends Gamepiece {

    // You will need to change this value depending on the year's game piece
    private static final double kGamepieceHeightInch = 7.0;

    /**
     * Holds the data determined by Coral
     *
     * @param box the array of points
     */
    public Ball(double[] box) {

      // You will need to tune these constants for this year's game piece
      this.distance = 231.13 * Math.pow(box[3] - box[1], -1.303);

      // This equation is constant between years. You only need to change the value of
      // the constant.
      this.xOffset = (160 - ((box[0] + box[2]) / 2)) / (((box[3] - box[1]) / kGamepieceHeightInch) * 39.37);
    }
  }

  public VisionSubsystem() {
    table = NetworkTableInstance.getDefault().getTable("ML");
    totalObjectsEntry = table.getEntry("nb_objects");
    classesEntry = table.getEntry("object_classes");
    boxesEntry = table.getEntry("boxes");
  }

  /**
   * Periodically updates the list of detected objects with the data found on
   * NetworkTables Also creates array of cargo and their relative position.
   */
  @Override
  public void periodic() {
    totalBalls = 0;
    totalObjects = (int) totalObjectsEntry.getDouble(0);
    classes = classesEntry.getStringArray(new String[totalObjects]);
    boxes = boxesEntry.getDoubleArray(new double[4 * totalObjects]);

    // Count up number of balls
    for (String s : classes) {
      if (s.equals("Ball"))
        totalBalls++;
    }

    balls = new Ball[totalBalls];
    int index = 0;

    // Generate array of Ball objects
    for (int i = 0; i < totalObjects; i += 4) {
      box = new double[4];
      for (int j = 0; j < 4; j++) {
        box[j] = boxes[i + j];
      }
      if (classes[i].equals("Ball")) {
        balls[index] = new Ball(box);
        index++;
      }
    }
  }

  public int getTotalBalls() {
    return totalBalls;
  }

  public Ball[] getBalls() {
    return balls;
  }
}
