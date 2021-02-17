// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.waypoint;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.spline.Spline.ControlVector;
import edu.wpi.first.wpilibj.trajectory.Trajectory;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator.ControlVectorList;

public final class WaypointUtil {
  private WaypointUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Imports Path object and creates a trajectory with that info.
   *
   * @param filepath Full path to the file
   * @param config TrajectoryConfig that sets the contraints for the Trajectory to be generated
   *
   * @return Trajectory created from the waypoints in the file.
   */
  public static Trajectory importPathToCubicTrajectory(String filepath, TrajectoryConfig config) throws IOException {
    try (FileReader fr = new FileReader(new File(filepath));
      BufferedReader reader = new BufferedReader(fr)) {
      
      var interiorWaypoints = new ArrayList<Translation2d>();
      Pose2d start = new Pose2d();
      Pose2d end = new Pose2d();
      int loop = 0;
      String line;
      String lastline = "";

      while ((line = reader.readLine()) != null) {
        if (loop == 0) {
          if (!line.equals("X,Y,Tangent X,Tangent Y,Fixed Theta,Reversed,Name")) {
            throw new RuntimeException("this isn’t a PathWeaver csv file");
          }  
        }
        else if (loop == 1) {
          start = createPoseWaypoint(line);
        }
        else if (loop == 2) {
          ; // skip the second line because we are logging the lastline and at 2 lastline is start
        }
        else {
          interiorWaypoints.add(createTranslationWaypoint(lastline));
        }
        lastline = line;
        loop++;
      }
      end = createPoseWaypoint(lastline);

      return TrajectoryGenerator.generateTrajectory(
            start,
            interiorWaypoints,
            end,
            config);
    }
  }

  /**
   * Imports Path object and creates a trajectory with that info.
   *
   * @param filepath Full path to the file
   * @param config TrajectoryConfig that sets the contraints for the Trajectory to be generated
   *
   * @return Trajectory created from the waypoints in the file.
   */
  public static Trajectory importPathToQuinticTrajectory(String filepath, TrajectoryConfig config) throws IOException {
    try (FileReader fr = new FileReader(new File(filepath));
      BufferedReader reader = new BufferedReader(fr)) {
      
      ControlVectorList controlVectors = new ControlVectorList();
      int loop = 0;
      String line;

      while ((line = reader.readLine()) != null) {
         if (loop == 0) {
            if (!line.equals("X,Y,Tangent X,Tangent Y,Fixed Theta,Reversed,Name")) {
              throw new RuntimeException("this isn’t a PathWeaver csv file");
            }
         }
         else {
            controlVectors.add(createControlVector(line));
         }
         loop++;
      }
      return TrajectoryGenerator.generateTrajectory(controlVectors,            
            config);
    }
  }

  private static Pose2d createPoseWaypoint(String input) {
    String[] arrOfStr = input.split(",", 0);
    return new Pose2d(new Translation2d(Double.parseDouble(arrOfStr[0]), Double.parseDouble(arrOfStr[1])),
       new Rotation2d(Double.parseDouble(arrOfStr[2]), Double.parseDouble(arrOfStr[3])));
  }

  private static Translation2d createTranslationWaypoint(String input) {
    String[] arrOfStr = input.split(",", 0);
    return new Translation2d(Double.parseDouble(arrOfStr[0]), Double.parseDouble(arrOfStr[1]));
  }

  private static ControlVector createControlVector(String input) {
    String[] arrOfStr = input.split(",", 0);
    double[] x = new double[] {Double.parseDouble(arrOfStr[0]), Double.parseDouble(arrOfStr[2])};
    double[] y = new double[] {Double.parseDouble(arrOfStr[1]), Double.parseDouble(arrOfStr[3])};
    return new ControlVector(x, y);
  }
}
